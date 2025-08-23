# Memory ordering

## `arch::mmio_space`

`arch::mmio_space` is intended to be used with device memory mappings.
If `arch::mmio_space` is used with other memory (e.g., main memory),
the ordering guarantees that libarch provides may be weaker than stated below.

### `load()` and `store()`

The `load()` and `store()` methods are intended to be suitable for most MMIO accesses that need to
be done in device drivers. libarch guarantees the following constraints for `load()` and `store()`:
1. All main memory accesses that occur _after_ `load()` in program order
    are ordered _after_ the `load()` access
    for all observers (i.e., all CPUs and devices).
2. All main memory accesses that occur _before_ `store()` in program order
    are ordered _before_ the `store()` access
    for all observers (i.e., all CPUs and devices).
3. `load()` and `store()` calls to the same device memory region are strongly ordered,
    i.e., all observers observe all `load()` and `store()` accesses to the same device
    memory region in the same order.
    It is architecture-, bus-, and device-specific what a memory region is.
4. `load()` interacts as expected (i.e., as if it was a main memory read)
    with preceding load-acquire reads from main memory.
    Likewise, `store()` interacts as expected (i.e., as if it was a main memory write)
    with following store-release writes to main memory.

**Implications** of these constraints are:
- Since constraint 1 orders main memory reads that follow `load()` in program order,
    it is safe to read data written by a device-to-host transfer
    from a DMA buffer after reading a device status register.
- Since constraint 2 orders main memory writes that precede `store()` in program order,
    it is safe to write a device doorbell register
    after filling the DMA buffers of a host-to-device transfer.
- The fact that constraint 1 also orders main memory writes that follow `load()` in program order
    helps to prevent some surprises. For example, some devices require load accesses
    to clear status conditions.
    Likewise, since constraint 2 also orders main memory reads that precede store()
    in program order,
    reusing a DMA buffer for device-to-host transfers after reading from the buffer
    does not require additional barriers.
- Due to constraint 4, `load()` and `store()` cannot be moved out of (or into) mutexes.

**Caveats.** Note that constraints 1 and 2 only order `load()` and `store()` relative to accesses
to main memory. They do not affect the order of `load()` and `store()` relative to each other.
Some examples of access patterns that are not covered by the guarantees above are:
- Main memory accesses before `load()` or after `store()`:
    - A read from main memory followed by a `load()` to device memory
        (unless the read is a load-acquire).
    - A write to main memory followed by `load()` of device memory.
    - A `store()` to device memory followed by a read from main memory.
    - A `store()` of device memory followed by a write to main memory
        (unless the write is a store-release).
- `load()` or `store()` to a device A followed by `load()` or `store()` to a another device B.
- Ordering of `load()` against preceding store-release to main memory.
    Likewise, ordering of `store()` against following load-acquire from main memory.

Also note that while `load()` and `store()` provide ordering guarantees
relative to main memory accesses, they do not guarantee visibility of main memory
accesses for all observers. In particular, explicit cache flushes (see `arch::dma_barrier`)
are necessary to ensure that main memory accesses are visible to non-coherent observers.

**Implementation.**
The following table depicts the architecture-specific barriers that are required
to guarantee the constraints of `load()` and `store()`:

|Architecture|Method|Before access|After access|
|---|---|---|---|
|Aarch64|`load()`||`dmb oshld`¹|
|Aarch64|`store()`|`dmb osh`¹|
|RISC-V|`load()`|`fence r, i`²|`fence i, rw`|
|RISC-V|`store()`|`fence rw, o`|`fence o, w`²|

¹ `dmb oshld` is enough to implement `read()` since it orders reads vs. reads and writes.
On the other hand, `store()` requires `dmb osh`
since `dmb oshst` only orders writes vs. writes.

² `fence r, i` is required to order the `load()` access vs. earlier load-acquire on main memory.
Likewise, `fence, o, w` is required to order the `store()` access
vs. future store-release on main memory.

**Rationale.**
An alternative model could weaken the constraints to only require ordering
of main memory reads in constraint 1 and ordering of main memory writes in constraint 2.
We opt for the stronger model instead since it can prevent surprising behavior
and we consider the extra barriers to be insignificant compared to the overall
costs of MMIO accesses. In fact, on RISC-V, there is no difference in required barriers.
On Aarch64, the only difference is the use of the
stronger `dmb osh` for `store()` compared to the weaker `dmb oshst`
(with no difference in the implementation of `load()`).

### `load_relaxed()` and `store_relaxed()`

`load_relaxed()` and `store_relaxed()` methods are intended for situations in
which ordering of device access and main memory access is either not relevant
or ensured by explicit barriers.

Out of the properties that libarch guarantees for `load()` and `store()`,
only constraint 3 (i.e., strong ordering of accesses to the same device memory region)
is guaranteed for `load_relaxed()` and `store_relaxed()`.

**Caveats.** Note that this means that `load_relaxed()` and `store_relaxed()`
may be moved out of (or into) mutexes if no extra barriers are used.
