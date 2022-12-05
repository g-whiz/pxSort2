from numba import cfunc, carray
import pxsort


@cfunc(pxsort.map_function_signature())
def array_reverse(a_in, m, a_out, n):
    in_array = carray(a_in, (m,))
    out_array = carray(a_out, (n,))
    count = min(m, n)
    for i in range(count):
        out_array[i] = in_array[(count - 1) - i]


def test_jit_callback():
    m = pxsort.Map(array_reverse.address, 10, 10)
    lst_in = [float(i) for i in range(10)]
    lst_out = m(lst_in)
    assert lst_out == lst_in[::-1]
