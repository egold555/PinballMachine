// Shared by index.html and settings.html.

const $ = (id) => document.getElementById(id);

function el(tag, props = {}, ...children) {
    const e = Object.assign(document.createElement(tag), props);
    e.append(...children);
    return e;
}

async function post(url, body) {
    const res = await fetch(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body ?? {}),
    });
    return res.json();
}

// Fetches the machine's state every second, keeps the header's connection
// dot up to date, and hands the state to render(). Returns a function that
// refreshes straight away (after a change).
function poll(render) {
    async function refresh() {
        try {
            const state = await (await fetch('/api/state')).json();
            $('dot').className = 'dot' + (state.megaConnected ? ' on' : '');
            $('status').textContent = state.megaConnected ? 'machine connected' : 'machine not connected';
            render(state);
        } catch {
            $('dot').className = 'dot';
            $('status').textContent = 'Pi not responding';
        }
    }
    refresh();
    setInterval(refresh, 1000);
    return refresh;
}
