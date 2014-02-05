import types

COMMONTYPES = [getattr(types, t) for t in dir(types) \
               if not t.startswith('_') \
               and t not in ('ClassType', 'InstanceType', 'ModuleType')]

DOCTYPES = ('BuiltinFunctionType', 'BuiltinMethodType', 'ClassType',
            'FunctionType', 'GeneratorType', 'InstanceType',
            'LambdaType', 'MethodType', 'ModuleType',
            'UnboundMethodType', 'method-wrapper')

SIMPLETYPES = [getattr(types, t) for t in dir(types) \
               if not t.startswith('_') and t not in DOCTYPES]

del t


try:
    COMMONTYPES.append(type(''.__repr__))  # Method-wrapper in version 2.2.x.
except AttributeError:
    pass
