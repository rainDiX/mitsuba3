#pragma once

#include <mitsuba/core/stream.h>
#include <mitsuba/core/logger.h>

NAMESPACE_BEGIN(mitsuba)

/** \brief \ref Stream implementation that never writes to disk, but keeps track
 * of the size of the content being written.
 * It can be used, for example, to measure the precise amount of memory needed
 * to store serialized content.
 */
class MTS_EXPORT_CORE DummyStream : public Stream {
public:

    DummyStream()
        : Stream(), m_size(0), m_pos(0) { }

    /// Returns a string representation
    std::string toString() const override;

    // =========================================================================
    //! @{ \name Implementation of the Stream interface
    // =========================================================================
protected:

    /// Always throws, since DummyStream is write-only.
    // TODO: could we just not offer it from the interface? (e.g. read/write trait or such)
    virtual void read(void *, size_t) override {
        Log(EError, "DummyStream does not support reading.");
    }

    /// Does not actually write anything, only updates the stream's position and size.
    virtual void write(const void *, size_t size) override {
        m_size = std::max(m_size, m_pos + size);
        m_pos += size;
    }

public:

    /** \brief Updates the current position in the stream.
     * Even though the <tt>DummyStream</tt> doesn't write anywhere, position is
     * taken into account to accurately compute the size of the stream.
     */
    virtual void seek(size_t pos) override {
        m_pos = pos;
    }

    /** \brief Simply sets the current size of the stream.
     * The position is updated to <tt>min(old_position, size)</tt>.
     */
    virtual void truncate(size_t size) override {
        m_size = size;  // No underlying data, so there's nothing else to do.
        m_pos = std::min(m_pos, size);
    }

    /// Returns the current position in the stream.
    virtual size_t getPos() override { return m_pos; }

    /// Returns the size of the stream.
    virtual size_t getSize() override { return m_size; }

    /// No-op for <tt>DummyStream</tt>.
    virtual void flush() override { /* Nothing to do */ }

    /// Always returns true.
    virtual bool canWrite() const override { return true; }

    /** \brief Always returns false, as nothing written to a
     * <tt>DummyStream</tt> is actually written.
     */
    virtual bool canRead() const override { return false; }

    //! @}
    // =========================================================================

    MTS_DECLARE_CLASS()

protected:

    /// Trivial destructor.
    virtual ~DummyStream() = default;

private:
    /// Size of all data written to the stream
    size_t m_size;
    /** \brief Current position in the "virtual" stream (even though nothing
     * is ever written, we need to maintain consistent positioning).
     */
    size_t m_pos;

};

NAMESPACE_END(mitsuba)
