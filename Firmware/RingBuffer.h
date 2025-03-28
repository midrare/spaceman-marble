#ifndef RINGBUFFER_H17178636
#define RINGBUFFER_H17178636

#if defined(__cplusplus) && (__cplusplus >= 201703L)
#if __has_include(<utility>)
#include <utility>
#endif
#endif

#include <stddef.h>


template <typename T, size_t MAX> class RingBuffer {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    class iterator {
    public:
        using iterator_category = void; // simplified for Arduino
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(RingBuffer* rb, size_type index) : rb(rb), index(index) {
        }

        reference operator*() const {
            return rb->buffer[(rb->start + index) % rb->size_];
        }

        pointer operator->() const {
            return &**this;
        }

        iterator& operator++() {
            ++index;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++index;
            return tmp;
        }

        iterator& operator--() {
            --index;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --index;
            return tmp;
        }

        iterator& operator+=(difference_type n) {
            index += n;
            return *this;
        }

        iterator& operator-=(difference_type n) {
            index -= n;
            return *this;
        }

        iterator operator+(difference_type n) const {
            return iterator(rb, index + n);
        }

        iterator operator-(difference_type n) const {
            return iterator(rb, index - n);
        }

        difference_type operator-(const iterator& other) const {
            return index - other.index;
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        bool operator==(const iterator& other) const {
            return rb == other.rb && index == other.index;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        bool operator<(const iterator& other) const {
            return index < other.index;
        }

        bool operator>(const iterator& other) const {
            return index > other.index;
        }

        bool operator<=(const iterator& other) const {
            return index <= other.index;
        }

        bool operator>=(const iterator& other) const {
            return index >= other.index;
        }

    private:
        RingBuffer* rb = NULL;
        size_type index = 0;
    };

    class const_iterator {
    public:
        using iterator_category = void; // simplified for Arduino
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const RingBuffer* rb, size_type index)
            : rb(rb), index(index) {
        }

        reference operator*() const {
            return rb->buffer[(rb->start + index) % rb->size_];
        }

        pointer operator->() const {
            return &**this;
        }

        const_iterator& operator++() {
            ++index;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++index;
            return tmp;
        }

        const_iterator& operator--() {
            --index;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --index;
            return tmp;
        }

        const_iterator& operator+=(difference_type n) {
            index += n;
            return *this;
        }

        const_iterator& operator-=(difference_type n) {
            index -= n;
            return *this;
        }

        const_iterator operator+(difference_type n) const {
            return const_iterator(rb, index + n);
        }

        const_iterator operator-(difference_type n) const {
            return const_iterator(rb, index - n);
        }

        difference_type operator-(const const_iterator& other) const {
            return index - other.index;
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        bool operator==(const const_iterator& other) const {
            return rb == other.rb && index == other.index;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        bool operator<(const const_iterator& other) const {
            return index < other.index;
        }

        bool operator>(const const_iterator& other) const {
            return index > other.index;
        }

        bool operator<=(const const_iterator& other) const {
            return index <= other.index;
        }

        bool operator>=(const const_iterator& other) const {
            return index >= other.index;
        }

    private:
        const RingBuffer* rb = NULL;
        size_type index = 0;
    };

    RingBuffer() {
        memset(&buffer, 0x00, sizeof(buffer));
    }

    RingBuffer(const RingBuffer& other)
        : start(other.start), size_(other.size_) {
        memset(&buffer, 0x00, sizeof(buffer));
        for (size_type i = 0; i < size_; ++i) {
            buffer[(start + i) % MAX] = other.buffer[(other.start + i) % MAX];
        }
    }

    RingBuffer(RingBuffer&& other) noexcept
        : start(other.start), size_(other.size_) {
        memset(&buffer, 0x00, sizeof(buffer));

        for (size_type i = 0; i < size_; ++i) {
            buffer[(start + i) % MAX] = other.buffer[(other.start + i) % MAX];
        }

        other.start = 0;
        other.size_ = 0;
    }

    RingBuffer& operator=(const RingBuffer& other) {
        if (this != &other) {
            start = other.start;
            size_ = other.size_;
            for (size_type i = 0; i < size_; ++i) {
                buffer[(start + i) % MAX] =
                    other.buffer[(other.start + i) % MAX];
            }
        }
        return *this;
    }

    RingBuffer& operator=(RingBuffer&& other) noexcept {
        if (this != &other) {
            start = other.start;
            size_ = other.size_;
            for (size_type i = 0; i < size_; ++i) {
                buffer[(start + i) % MAX] =
                    other.buffer[(other.start + i) % MAX];
            }
            other.start = 0;
            other.size_ = 0;
        }
        return *this;
    }

    ~RingBuffer() = default;

    size_type size() const {
        return size_;
    }

    size_type max_size() const {
        return MAX;
    }

    bool empty() const {
        return size_ == 0;
    }

    reference operator[](size_type i) {
        return buffer[(start + i) % MAX];
    }

    const_reference operator[](size_type i) const {
        return buffer[(start + i) % MAX];
    }

    reference front() {
        return buffer[start];
    }

    const_reference front() const {
        return buffer[start];
    }

    reference back() {
        return buffer[(start + size_ - 1) % MAX];
    }

    const_reference back() const {
        return buffer[(start + size_ - 1) % MAX];
    }

    void push_back(const T& value) {
        if (size_ >= MAX) {
            pop_front();
        }
        buffer[(start + size_) % MAX] = value;
        if (size_ < MAX) {
            size_++;
        }
    }

    template <typename... Args> void emplace_back(Args&&... args) {
        if (size_ >= MAX) {
            pop_front();
        }
        buffer[(start + size_) % MAX] = T(args...);
        if (size_ < MAX) {
            size_++;
        }
    }

    void pop_front() {
        if (size_ > 0) {
            start = (start + 1) % MAX;
            size_--;
        }
    }

    void push_front(const T& value) {
        if (size_ >= MAX) {
            pop_back();
        }

        start = ((start + MAX) - 1) % MAX;
        buffer[start] = value;
        if (size_ < MAX) {
            size_++;
        }
    }

    void pop_back() {
        if (size_ > 0) {
            size_--;
        }
    }

    iterator insert(iterator pos, const T& value) {
        if (size_ >= MAX) {
            return end();
        }
        size_t insert_index = pos - begin();
        for (size_t k = size_; k > insert_index; --k) {
            buffer[(start + k) % MAX] = buffer[(start + k - 1) % MAX];
        }
        buffer[(start + insert_index) % MAX] = value;
        size_++;
        return iterator(this, insert_index);
    }

    iterator erase(iterator pos) {
        if (pos == end())
            return end();
        size_t erase_index = pos - begin();
        for (size_t k = erase_index; k < size_ - 1; ++k) {
            buffer[(start + k) % MAX] = buffer[(start + k + 1) % MAX];
        }
        size_--;
        return iterator(this, erase_index);
    }

    iterator erase(iterator first, iterator last) {
        if (first == last || first == end())
            return end();
        size_t start_idx = first - begin();
        size_t end_idx = last - begin();
        if (end_idx > size_)
            end_idx = size_;
        size_t count = end_idx - start_idx;
        if (count == 0)
            return iterator(this, start_idx);
        for (size_t k = start_idx; k < size_ - count; ++k) {
            buffer[(start + k) % MAX] = buffer[(start + k + count) % MAX];
        }
        size_ -= count;
        return iterator(this, start_idx);
    }

    void clear() {
        size_ = 0;
    }

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, size_);
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, size_);
    }

    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }

    const_iterator cend() const {
        return const_iterator(this, size_);
    }

private:
    T buffer[MAX];
    size_type start = 0;
    size_type size_ = 0;
};

#endif // RINGBUFFER_H17178636
