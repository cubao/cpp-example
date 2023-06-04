# cpp-example

```
make build
make test
```

How to keep original json key order?


{
    "key1": ...,
    "key2": [5, {"key3": ..., "key4": ...}],
}

-> [
    ["key1"],
    ["key2", [
        [],
        ["key3", []]
    ]],
]
