import sys

class MissingModule:
    _NOT_IMPLEMENTED_ = True

    def __init__(self, name, urgent=0):
        self.name = name
        exc_type, exc_msg = sys.exc_info()[:2]
        self.info = str(exc_msg)
        self.reason = f"{exc_type.__name__}: {self.info}"
        self.urgent = urgent
        if urgent:
            self.warn()

    def __getattr__(self, var):
        if not self.urgent:
            self.warn()
            self.urgent = 1
        missing_msg = f"{self.name} module not available ({self.reason})"
        raise NotImplementedError(missing_msg)

    def __bool__(self):
        return False

    def warn(self):
        msg_type = "import" if self.urgent else "use"
        message = f"{msg_type} {self.name}: {self.info}\n({self.reason})"
        try:
            import warnings

            level = 4 if self.urgent else 3
            warnings.warn(message, RuntimeWarning, level)
        except ImportError:
            print(message)

try:
    import testmod.mod_py
    from testmod.mod_py import test_cls_py
except (ImportError, OSError):
    mod_py = MissingModule("mod_py", urgent=1)

try:
    import testmod.mod_c
    from testmod.mod_c import test_cls_c
except (ImportError, OSError):
    mod_c = MissingModule("mod_c", urgent=1)
