| Scenario                                                | Strategy                          | When to Use                                                     | Notes                                                    |
|----------------------------------------------------------|-----------------------------------|------------------------------------------------------------------|----------------------------------------------------------|
| 1. Array of POD (e.g. int, float) with constant values   | `llvm.memcpy` or per-element `store` | Elements known at compile-time                                   | Efficient and simple                                     |
| 2. Array of POD with default (zero) initialization       | `llvm.memset`                     | Default-init to zero, trivial types                              | Common for C-style default init                          |
| 3. Array of trivially constructible structs              | `llvm.memset` or `llvm.memcpy`   | No constructor/destructor                                        | Treated like POD                                         |
| 4. Array of non-trivial objects (has ctor/dtor)          | Loop + constructor `call`        | Class with user-defined constructor or destructor                | Must emit ctor at correct address                        |
| 5. Array of pointers or references                       | Per-element `store`              | Initialized individually or dynamically                          | Can't use memcpy safely                                  |
| 6. Array copied from another variable                    | `llvm.memcpy` or loop + copy ctor `call` | Only if copy is trivial or suppressed                    | Must call copy ctor if present                          |
| 7. Array partially initialized (e.g. `{1, 2}`)           | `store` for values, `memset` for rest | C-style partial init                                         | Follows C/C++ default rules                             |
| 8. Array of objects with non-trivial copy constructor    | Loop + copy constructor `call`   | Copying from another array or object                             | Important to preserve semantics                          |

- For constants → memcpy or store.

- For runtime values → Per-element store.

- For nontrivial object → Call constructor. 