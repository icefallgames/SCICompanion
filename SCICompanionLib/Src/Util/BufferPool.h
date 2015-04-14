#pragma once

#pragma warning(disable: 4351)      // New behavior for arrays in initializer list.

template<size_t BufferSize, int MaxBuffer>
class BufferPool
{
public:
    BufferPool() : _used{} {}

    uint8_t *AllocateBuffer()
    {
        for (size_t i = 0; i < ARRAYSIZE(_buffers); i++)
        {
            if (!_used[i])
            {
                if (!_buffers[i])
                {
                    _buffers[i] = std::make_unique<uint8_t[]>(BufferSize);
                }
                _used[i] = true;
                return _buffers[i].get();
            }
        } 
        assert(false && "Requesting more buffers than indicated.");
        return nullptr;
    }

    void FreeBuffer(uint8_t *buffer)
    {
        if (buffer)
        {
            for (size_t i = 0; i < ARRAYSIZE(_buffers); i++)
            {
                if (buffer == _buffers[i].get())
                {
                    _used[i] = false;
                    break;
                }
            }
        }
    }

private:
    std::unique_ptr<uint8_t[]> _buffers[MaxBuffer];
    bool _used[MaxBuffer];
    int _maxBuffers;
    size_t _size;
};