#include "SoundPlayer.h"

void SoundPlayer::begin(MegaLink &link)
{
    link_ = &link;
    xTaskCreate(taskEntry, "sound", 3072, this, 4, &task_);
}

void SoundPlayer::play(const uint8_t *score, SoundPriority priority)
{
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (playing_ && priority_ == SoundPriority::Normal)
            return;
        score_ = score;
        priority_ = priority;
        playing_ = true;
        restart_ = true;
    }
    xTaskNotifyGive(task_);
}

void SoundPlayer::stop()
{
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (!playing_)
            return;
        playing_ = false;
    }
    uint8_t all = 0xFF;
    link_->send(pb::MSG_NOTE_OFF, &all, 1);
    xTaskNotifyGive(task_);
}

bool SoundPlayer::playing()
{
    std::lock_guard<std::mutex> lock(lock_);
    return playing_;
}

void SoundPlayer::taskEntry(void *self)
{
    static_cast<SoundPlayer *>(self)->run();
}

void SoundPlayer::run()
{
    for (;;)
    {
        TickType_t wait = portMAX_DELAY;
        {
            std::lock_guard<std::mutex> lock(lock_);
            if (restart_)
            {
                restart_ = false;
                uint8_t all = 0xFF;
                link_->send(pb::MSG_NOTE_OFF, &all, 1);
                cursor_ = score_;
                nextStep_ = millis();
            }
            if (playing_ && (int32_t)(millis() - nextStep_) >= 0)
                step();
            if (playing_)
            {
                int32_t ms = nextStep_ - millis();
                wait = pdMS_TO_TICKS(ms > 0 ? ms : 0);
            }
        }
        ulTaskNotifyTake(pdTRUE, wait); // a play() or stop() wakes us early
    }
}

// Runs score commands until the next wait. Caller holds lock_.
void SoundPlayer::step()
{
    for (;;)
    {
        uint8_t cmd = *cursor_++;
        if (cmd < 0x80)
        {
            nextStep_ += ((cmd << 8) | *cursor_++);
            return;
        }

        uint8_t chan = cmd & 0x0F;
        switch (cmd & 0xF0)
        {
        case 0x90:
        {
            uint8_t note = *cursor_++;
            if (chan < pb::SOUND_CHANNELS)
            {
                uint8_t payload[2] = {chan, note};
                link_->send(pb::MSG_NOTE_ON, payload, 2);
            }
            break;
        }
        case 0x80:
            if (chan < pb::SOUND_CHANNELS)
                link_->send(pb::MSG_NOTE_OFF, &chan, 1);
            break;
        case 0xE0:
            cursor_ = score_;
            break;
        default: // 0xF0 end, or garbage
        {
            uint8_t all = 0xFF;
            link_->send(pb::MSG_NOTE_OFF, &all, 1);
            playing_ = false;
            return;
        }
        }
    }
}
