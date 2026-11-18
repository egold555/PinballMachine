import assert from 'node:assert/strict';
import { copyFileSync, mkdirSync, mkdtempSync, readFileSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { test } from 'node:test';

process.env.PINBALL_DATA_DIR = mkdtempSync(join(tmpdir(), 'pinball-test-'));

const { midiToScore } = await import('../src/sounds/midi.js');
const { SoundBank, loadSounds, soundInfo } = await import('../src/sounds/library.js');
const builtIn = await import('../src/sounds/scores.js');

const SOUNDS = join(import.meta.dirname, '..', 'sounds', 'original');

// A one-track MIDI file at 120 bpm, 480 ticks per beat (1 tick = 1.0417 ms).
// Events: [delta ticks, status, data...]
function makeMidi(events: number[][], tempo?: number): Uint8Array {
    const body: number[] = [];
    if (tempo) body.push(0, 0xff, 0x51, 3, (tempo >> 16) & 0xff, (tempo >> 8) & 0xff, tempo & 0xff);
    for (const [delta, ...rest] of events) {
        const v = [delta & 0x7f];
        for (let d = delta >> 7; d; d >>= 7) v.unshift((d & 0x7f) | 0x80);
        body.push(...v, ...rest);
    }
    body.push(0, 0xff, 0x2f, 0);
    const len = body.length;
    return Uint8Array.from([
        ...Buffer.from('MThd'), 0, 0, 0, 6, 0, 0, 0, 1, 0x01, 0xe0,
        ...Buffer.from('MTrk'), (len >> 24) & 0xff, (len >> 16) & 0xff, (len >> 8) & 0xff, len & 0xff, ...body,
    ]);
}

test('a simple tune converts exactly', () => {
    // note 60 for 480 ticks (500 ms), then note 64 for 480 ticks
    const r = midiToScore(makeMidi([[0, 0x90, 60, 100], [480, 0x80, 60, 0], [0, 0x90, 64, 100], [480, 0x80, 64, 0]]));
    assert.deepEqual([...r.score], [0x90, 60, 0x01, 0xf4, 0x80, 0x90, 64, 0x01, 0xf4, 0x80, 0xf0]);
    assert.equal(r.ms, 1000);
});

test('tempo changes are followed', () => {
    const r = midiToScore(makeMidi([[0, 0x90, 60, 100], [480, 0x80, 60, 0]], 1_000_000)); // 60 bpm
    assert.equal(r.ms, 1000);
});

test('more than three notes keeps the highest and lowest', () => {
    // C E G held, then a high C: the oldest middle note (E) gives up its voice
    const r = midiToScore(
        makeMidi([
            [0, 0x90, 60, 100], [0, 0x90, 64, 100], [0, 0x90, 67, 100],
            [10, 0x90, 72, 100],
            [480, 0x80, 60, 0], [0, 0x80, 64, 0], [0, 0x80, 67, 0], [0, 0x80, 72, 0],
        ]),
    );
    assert.equal(r.notes, 4);
    assert.equal(r.dropped, 1);
    const s = [...r.score];
    assert.deepEqual(s.slice(0, 6), [0x90, 60, 0x91, 64, 0x92, 67]);
    assert.deepEqual(s.slice(8, 10), [0x91, 72], 'high C takes E\'s voice');
});

test('a SysEx message before the notes does not shift the timing', () => {
    const sysex = [0xf0, 4, 0x41, 0x10, 0x42, 0xf7];
    const r = midiToScore(makeMidi([[0, ...sysex], [0, 0x90, 60, 100], [480, 0x80, 60, 0]]));
    assert.deepEqual([...r.score], [0x90, 60, 0x01, 0xf4, 0x80, 0xf0]);
});

test('an "end" marker sets the length and the tempo is read', () => {
    // note for 480 ticks, then a marker 480 ticks later: 1 s long at 120 bpm
    const marker = [0xff, 0x06, 3, ...Buffer.from('end')];
    const r = midiToScore(makeMidi([[0, 0x90, 60, 100], [480, 0x80, 60, 0], [480, ...marker]]));
    assert.equal(r.ms, 1000);
    assert.equal(r.bpm, 120);
    assert.equal(midiToScore(makeMidi([[0, 0x90, 60, 100], [480, 0x80, 60, 0]], 400_000)).bpm, 150);
});

test('drums are left out', () => {
    const r = midiToScore(makeMidi([[0, 0x99, 36, 100], [0, 0x90, 60, 100], [100, 0x89, 36, 0], [0, 0x80, 60, 0]]));
    assert.equal(r.notes, 1);
});

test('the real sound files match the old built-in sounds', () => {
    const notes = (f: string) => midiToScore(readFileSync(join(SOUNDS, f))).notes;
    assert.equal(notes('start-game.mid'), 15);
    assert.equal(notes('new-ball.mid'), 28);
    assert.equal(notes('extra-ball.mid'), 6);
    assert.equal(notes('tilt.mid'), 6);
    assert.equal(notes('point.mid'), 1);
});

test('each mode reads its own folder, with built-in or silent fallbacks', () => {
    const root = mkdtempSync(join(tmpdir(), 'pinball-sounds-'));
    const a = new SoundBank('mode-a', ['song', 'tilt', 'gone'] as const, { tilt: builtIn.SOUND_TILT });
    const b = new SoundBank('mode-b', ['song'] as const);
    mkdirSync(join(root, 'mode-a'));
    copyFileSync(join(SOUNDS, 'ending-fireball.mid'), join(root, 'mode-a', 'Song.MID'));
    writeFileSync(join(root, 'mode-a', 'tilt.mid'), 'not really midi');
    loadSounds(root);

    assert.ok(a.get('song'), 'loaded, case ignored');
    assert.equal(b.get('song'), null, "mode-b has no folder: another mode's file is not used");
    assert.equal(a.get('tilt'), builtIn.SOUND_TILT, 'unreadable file: built-in');
    assert.equal(a.get('gone'), null, 'no file, no built-in: silent');
    const tilt = soundInfo().find((s) => s.mode === 'mode-a' && s.name === 'tilt')!;
    assert.equal(tilt.source, 'built-in');
    assert.match(tilt.error ?? '', /not a MIDI file/);
});

test('a looping sound repeats until stopped', async () => {
    const { SoundPlayer } = await import('../src/SoundPlayer.js');
    const { setTimeout: delay } = await import('node:timers/promises');
    let noteOns = 0;
    const link = { send: (type: number) => void (type === 0x17 && noteOns++) };
    const player = new SoundPlayer(link as never);

    player.play(Uint8Array.from([0x90, 60, 0, 50, 0x80, 0xf0]), { loop: true }); // 50 ms, then 1 s pause
    await delay(1200);
    assert.equal(noteOns, 2, 'played twice');
    assert.ok(player.playing);

    player.stop();
    await delay(1100);
    assert.equal(noteOns, 2, 'nothing after stop');
    assert.ok(!player.playing);

    player.play(Uint8Array.from([0xf0]), { loop: true }); // empty: must not hang
    await delay(20);
    player.stop();
});

test('a gapless loop comes straight back, and an empty one can\'t hang', async () => {
    const { SoundPlayer } = await import('../src/SoundPlayer.js');
    const { setTimeout: delay } = await import('node:timers/promises');
    let noteOns = 0;
    const player = new SoundPlayer({ send: (type: number) => void (type === 0x17 && noteOns++) } as never);

    player.play(Uint8Array.from([0x90, 60, 0, 100, 0xf0]), { loop: true, loopGapMs: 0 }); // 100 ms
    await delay(350);
    player.stop();
    assert.ok(noteOns >= 3, `no pause between repeats (${noteOns} in 350 ms)`);

    player.play(Uint8Array.from([0xf0]), { loop: true, loopGapMs: 0 }); // would spin forever
    await delay(20);
    assert.ok(!player.playing, 'stops instead');
});
