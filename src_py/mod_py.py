class test_cls_py:
    def __init__(self, x: int):
        self.x = x
        print(f"Initialized with len {self.x}")
    def __len__(self):
        return self.x
    def __str__(self) -> str:
        return f"test class with len {self.x}"