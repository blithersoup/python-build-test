import testmod

x = testmod.mod_py.test_cls_py(5)

y = testmod.mod_c.test_cls_c("hello world")
print(str(y))
print(y)
print(repr(y))