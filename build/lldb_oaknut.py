import lldb

# Makes certain core Oaknut types look pretty in LLDB and the XCode debugger.

def __lldb_init_module(debugger, dict):
    # Logging code
    #lldb.formatters.Logger._lldb_formatters_debug_level = 2
    #lldb.formatters.Logger._lldb_formatters_debug_filename = "/Users/Shared/lldb.py.log"
    #logger = lldb.formatters.Logger.Logger()
    #logger >> "Var named " + str(self.valobj.GetName()) + " has realobj type " + self.array_obj.GetTypeName()

    debugger.HandleCommand('type summary add -F lldb_oaknut.variant_SummaryProvider oak::variant')
    debugger.HandleCommand('type summary add -F lldb_oaknut.bytearray_SummaryProvider oak::bytearray')
    debugger.HandleCommand('type summary add -F lldb_oaknut.string_SummaryProvider oak::string')
    debugger.HandleCommand('type summary add -F lldb_oaknut.measurement_SummaryProvider oak::measurement')
    debugger.HandleCommand('type synthetic add oak::variant --python-class lldb_oaknut.variant_SynthProvider')
    debugger.HandleCommand('type summary add -F lldb_oaknut.style_SummaryProvider oak::style')
    debugger.HandleCommand('type summary add -F lldb_oaknut.stylemap_SummaryProvider "const std::__1::map<oak::string, oak::style, std::__1::less<oak::string>, std::__1::allocator<std::__1::pair<const oak::string, oak::style> > > &"')
    debugger.HandleCommand('type synthetic add oak::style --python-class lldb_oaknut.style_SynthProvider')

    # String-keyed std::pairs
    debugger.HandleCommand('type summary add -F lldb_oaknut.pair_SummaryProvider -x "std::__1::pair<const oak::string,.*"')
    debugger.HandleCommand('type synthetic add "std::__1::pair<const oak::string, oak::variant>" --python-class lldb_oaknut.pair_variant_SynthProvider')
    debugger.HandleCommand('type synthetic add "std::__1::pair<const oak::string, oak::style>" --python-class lldb_oaknut.pair_style_SynthProvider')

    # Geometry types
    debugger.HandleCommand('type summary add -F lldb_oaknut.POINT_SummaryProvider oak::POINT')
    debugger.HandleCommand('type summary add -F lldb_oaknut.SIZE_SummaryProvider oak::SIZE')
    debugger.HandleCommand('type summary add -F lldb_oaknut.RECT_SummaryProvider oak::RECT')

    debugger.HandleCommand('type summary add -F lldb_oaknut.COLOR_SummaryProvider oak::COLOR')
    debugger.HandleCommand('type synthetic add oak::COLOR --python-class lldb_oaknut.COLOR_SynthProvider')

    debugger.HandleCommand('type summary add -F lldb_oaknut.sp_SummaryProvider -x "^oak::sp<.*>"')
    debugger.HandleCommand('type synthetic add -x "^oak::sp<.*>" --python-class lldb_oaknut.sp_SynthProvider')

def nicetype(t):
    t = t.strip('oak::')
    t = t.replace(' *', '*')
    return t

def sp_SummaryProvider(valobj, dict):
    obj = valobj.GetNonSyntheticValue().GetChildAtIndex(0, lldb.eNoDynamicValues, True)
    if obj is None:
        return 'null'
    if obj.GetValueAsUnsigned() == 0:
        return 'null'
    v = str(obj.GetValue())
    while v.startswith('0x0'):
        v = v[0:2] + v[3:]
    return nicetype(obj.GetDisplayTypeName()) + ' ' + v

class sp_SynthProvider:
    def __init__(self, valobj, dict):
        self.valobj = valobj
    def num_children(self):
        if self.obj is None:
            return 0
        return self.obj.GetNumChildren()
    def get_child_index(self, name):
        if self.obj is None:
            return -1
        return self.obj.GetIndexOfChildWithName(name)
    def get_child_at_index(self, index):
        if self.obj is None:
            return None
        return self.obj.GetChildAtIndex(index)
    def has_children(self):
        if self.obj is None:
            return False
        return self.obj.MightHaveChildren()
    def update(self):
        self.obj = self.valobj.GetChildAtIndex(0, lldb.eNoDynamicValues, True)
        #if self.obj != None:
        #    self.obj = self.obj.Dereference()
        #    self.obj.update()

class variant_SynthProvider:
    def __init__(self, valobj, dict):
        self.valobj = valobj
    def num_children(self):
        return self.child_count
    def get_child_index(self, name):
        return None
    def get_child_at_index(self, index):
        c = self.container.GetChildAtIndex(index, lldb.eNoDynamicValues, True)
        if c == None:
            return None
        c = c.GetNonSyntheticValue() # we want the actual pair<string,variant> value
        name = None
        if self.type == 'MAP':
            name = c.GetChildMemberWithName('first')
            name = name.GetSummary()[1:-1]
        if name == None:
            name = '[' + str(index) + ']'
        return self.valobj.CreateValueFromData(str(name), c.GetData(), c.GetType());
    def update(self):
        self.type = self.valobj.GetChildMemberWithName('type').GetValue()
        self.child_count = 0
        self.container = None
        if self.type == 'BYTEARRAY':
            self.container = self.valobj.GetChildMemberWithName('_bytearray').Dereference()
            self.child_count = self.container.GetNumChildren()
        if self.type == 'ARRAY':
            self.container = self.valobj.GetChildMemberWithName('_vec').Dereference()
            self.child_count = self.container.GetNumChildren()
        if self.type == 'MAP':
            self.container = self.valobj.GetChildMemberWithName('_map').Dereference()
            self.child_count = self.container.GetNumChildren()
        return
    def has_children(self):
        return self.child_count > 0


def variant_SummaryProvider(valobj, dict):
    valobj = valobj.GetNonSyntheticValue()
    t = valobj.GetChildMemberWithName('type').GetValue()
    if t == 'EMPTY':
        return '<empty>'
    if t == 'INT32':
        return valobj.GetChildMemberWithName('_i32').GetValue() + 'l'
    if t == 'INT64':
        return valobj.GetChildMemberWithName('_i64').GetValue() + 'll'
    if t == 'UINT32':
        return hex(valobj.GetChildMemberWithName('_u32').GetValueAsUnsigned()) + 'ul'
    if t == 'UINT64':
        return hex(valobj.GetChildMemberWithName('_u64').GetValueAsUnsigned()) + 'ull'
    if t == 'FLOAT32':
        return valobj.GetChildMemberWithName('_f32').GetValue() + 'f'
    if t == 'FLOAT64':
        return valobj.GetChildMemberWithName('_f64').GetValue()
    if t == 'STRING':
        return valobj.GetChildMemberWithName('_str').GetSummary()
    if t == 'MEASUREMENT':
        return valobj.GetChildMemberWithName('_measurement').GetSummary()
    if t == 'BYTEARRAY':
        return valobj.GetChildMemberWithName('_bytearray').GetSummary()
    if t == 'ARRAY':
        return valobj.GetChildMemberWithName('_vec').Dereference().GetSummary()
    if t == 'MAP':
        c = valobj.GetChildMemberWithName('_map').Dereference().GetNumChildren()
        if c == 0:
            return '<empty>'
        if c == 1:
            return '1 entry'
        return str(c) + ' entries'
    return t

def pair_SummaryProvider(valobj, dict):
    valobj = valobj.GetNonSyntheticValue();
    return valobj.GetChildMemberWithName('second').GetSummary()

class pair_SynthProvider:
    def __init__(self, valobj, dict):
        self.valobj = valobj
    def num_children(self):
        if self.second == None:
            return 0
        return self.second.num_children()
    def get_child_index(self, name):
        if self.second == None:
            return -1
        return self.second.get_child_index(name)
    def get_child_at_index(self, index):
        if self.second == None:
            return None
        return self.second.get_child_at_index(index)
    def has_children(self):
        if self.second == None:
            return False
        return self.second.has_children()
    def update(self):
        self.second = self.updateSecond(self.valobj.GetChildMemberWithName('second'))
        if self.second != None:
            self.second.update()
    def updateSecond(self, s):
        pass


class pair_variant_SynthProvider(pair_SynthProvider):
    def updateSecond(self, s):
        return variant_SynthProvider(s, None)

class pair_style_SynthProvider(pair_SynthProvider):
    def updateSecond(self, s):
        return style_SynthProvider(s, None)


def bytearray_SummaryProvider(valobj, dict):
    s = valobj.GetChildMemberWithName('_cb')
    return 'len=' + s.GetValue()

def string_SummaryProvider(valobj, dict):
    buf = valobj.GetChildMemberWithName('_buf').GetValueAsUnsigned()
    if buf == 0:
        return '<null>'
    offset = valobj.GetChildMemberWithName('_offset').GetValueAsUnsigned()
    cb = valobj.GetChildMemberWithName('_cb').GetValueAsUnsigned()
    error = lldb.SBError()
    cstring = valobj.GetProcess().ReadCStringFromMemory(buf+offset, cb+1, error)
    if error.Success():
        return cstring
    return '<err: ' + error.GetCString() + '>'

def measurement_SummaryProvider(valobj, dict):
    r = valobj.GetChildMemberWithName('_unitVal').GetValue()
    u = valobj.GetChildMemberWithName('_unit').GetValue()
    if u == 'DP':
        return r + 'dp'
    if u == 'SP':
        return r + 'sp'
    if u == 'PX':
        return r + 'px'
    if u == 'PC':
        return r + '%'
    return r

def nicefloat(val):
    s = str(round(float(val), 3))
    if s.endswith('.0'):
        s = s[:-2]
    return s

def POINT_SummaryProvider(valobj, dict):
    x = nicefloat(valobj.GetChildMemberWithName('x').GetValue())
    y = nicefloat(valobj.GetChildMemberWithName('y').GetValue())
    return '{},{}'.format(x, y)

def SIZE_SummaryProvider(valobj, dict):
    width = nicefloat(valobj.GetChildMemberWithName('width').GetValue())
    height = nicefloat(valobj.GetChildMemberWithName('height').GetValue())
    return '{},{}'.format(width, height)

def RECT_SummaryProvider(valobj, dict):
    origin = valobj.GetChildMemberWithName('origin')
    x = nicefloat(origin.GetChildMemberWithName('x').GetValue())
    y = nicefloat(origin.GetChildMemberWithName('y').GetValue())
    size = valobj.GetChildMemberWithName('size')
    width = nicefloat(size.GetChildMemberWithName('width').GetValue())
    height = nicefloat(size.GetChildMemberWithName('height').GetValue())
    return '{},{} x {},{}'.format(x, y, width, height)

def COLOR_SummaryProvider(valobj, dict):
    return '#%x' % valobj.GetNonSyntheticValue().GetChildMemberWithName('_val').GetValueAsUnsigned()

class COLOR_SynthProvider:
    def __init__(self, valobj, dict):
        self.valobj = valobj
    def num_children(self):
        return 0
    def has_children(self):
        return False

def style_SummaryProvider(valobj, dict):
    valobj = valobj.GetNonSyntheticValue()
    t = valobj.GetChildMemberWithName('type').GetValue()
    if t == 'TypeSimple':
        valobj = valobj.GetChildMemberWithName('var')
        return valobj.GetSummary()
    if t == 'TypeArray':
        return valobj.GetChildMemberWithName('array').Dereference().GetSummary()
    if t == 'TypeCompound':
        c = valobj.GetChildMemberWithName('compound').Dereference().GetNumChildren()
        if c == 0:
            return '<empty>'
        if c == 1:
            return '1 entry'
        return str(c) + ' entries'
    return t

def stylemap_SummaryProvider(valobj, dict):
    valobj = valobj.GetNonSyntheticValue()
    c = valobj.GetNumChildren()
    if c == 0:
        return '<empty>'
    if c == 1:
        return '1 entry'
    return str(c) + ' entries'

class style_SynthProvider:
    def __init__(self, valobj, dict):
        self.valobj = valobj
    def num_children(self):
        return self.child_count
    def get_child_index(self, name):
        return None
    def get_child_at_index(self, index):
        c = self.container.GetChildAtIndex(index, lldb.eNoDynamicValues, True)
        if c == None:
            return None
        c = c.GetNonSyntheticValue() # we want the actual pair<string,variant> value
        name = None
        if self.type == 'TypeCompound':
            name = c.GetChildMemberWithName('first')
            name = name.GetSummary()
        if name == None:
            name = '[' + str(index) + ']'
        return self.valobj.CreateValueFromData(str(name), c.GetData(), c.GetType());
    def update(self):
        self.type = self.valobj.GetChildMemberWithName('type').GetValue()
        self.child_count = 0
        self.container = None
        if self.type == 'TypeSimple':
            self.container = self.valobj.GetChildMemberWithName('var')
        if self.type == 'TypeArray':
            self.container = self.valobj.GetChildMemberWithName('array').Dereference()
            self.child_count = self.container.GetNumChildren()
        if self.type == 'TypeCompound':
            self.container = self.valobj.GetChildMemberWithName('compound').Dereference()
            self.child_count = self.container.GetNumChildren()
        return
    def has_children(self):
        return self.child_count > 0
