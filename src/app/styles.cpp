//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

/*

 - Resources are a hierarchy of JSON-style key-value pairs but without quote marks around everything.
 - No commas needed to separate fields, field ends at newline unless it is a quoted string which is
   allowed to contain newlines and escaped characters.
 - Keys are always strings, values are [string|int|float|ref|array|map]
 - Keys are chainable, i.e. the hierarchy can be traversed by "key.subkey.value".
 - Values can refer to other values via a $ prefix.
 - Later declarations override earlier ones if they have exactly the same name.

*/

static style s_styleRoot;

static struct init {
    init() {
        string stylesres(
    #include "styles.res"
        );
        s_styleRoot.parse(stylesres);
    }
} _init;

// A qualifier is a uint32_t where the low byte is the name and the upper 3 bytes are a parameter.

#define MAKE_Q(name, param) uint32_t(((param)<<8) | (name))

#define Q_DEFAULT    0
#define Q_VIEWSTATE  1
#define Q_IOS        2
#define Q_MACOS      3
#define Q_ANDROID    4
#define Q_WEB        5
#define Q_LINUX      6
#define Q_WINDOWS    7
#define Q_DESKTOP    8
#define Q_MOBILE     9
#define Q_TABLET    10
#define Q_TV        11
#define Q_LANDSCAPE 32
#define Q_PORTRAIT  33
#define Q_DPI_1x    34
#define Q_DPI_2x    35
#define Q_DPI_3x    36
#define Q_SCREEN_WIDTH_LONGER 37
#define Q_SCREEN_WIDTH_SHORTER 38





void style::loadStyleAssetSync(const string& assetPath) {
    variant v = app->loadAssetSync(assetPath);
    if (v.isError()) {
    } else {
        auto& data = v.bytearrayRef();
        string str = data.toString();
        style s;
        s.parse(str);
        assert(s.isCompound());
        s_styleRoot.importNamedValues(*s.compound);
    }
}



const style* style::get(const string& keypath) {
    static map<string,const style*> styleCache;
    auto cacheVal = styleCache.find(keypath);
    if (cacheVal != styleCache.end()) {
        return cacheVal->second;
    }
    const style* val = s_styleRoot.getValue(keypath);
    styleCache.insert(std::make_pair(keypath, val));
    return val;
}

const style* style::resolve(const style* val) {

    while (val) {
            
        // Resolve references to other styles
        if (val->type == style::TypeReference) {
            val = val->reference;
        }
        else if (val->type == style::TypeSimple && val->var.isString() && val->var.stringVal().hasPrefix("$")) {
            auto refdstylename = val->var.stringVal();
            refdstylename.eraseAt(0, 1);
            auto refdstyle = style::get(refdstylename);
            if (!refdstyle) {
                log_warn("Missing referenced style: $%s", refdstylename.c_str());
            } else {
                style* ncval = const_cast<style*>(val);
                ncval->setType(TypeReference);
                ncval->reference = refdstyle;
            }
            val = refdstyle;
        }

        // Run-time selection of qualified value
        else if (val->type == style::TypeQual) {
            const style* newval = NULL;
            for (auto it = val->qualmap->begin() ; it != val->qualmap->end() ; it++) {
                uint32_t qual = it->first;
                bool applies = false;
                switch (qual & 0xFF) {
                    case Q_DEFAULT:
                        applies = true;
                        break;
                        
                    case Q_VIEWSTATE:
                        // Dynamic qualifiers cannot be resolved here!
                        return val;
                        
                    case Q_IOS:
#if PLATFORM_IOS
                        applies = true;
#endif
                        break;
                    case Q_MACOS:
#if PLATFORM_OSX
                        applies = true;
#endif
                        break;
                    case Q_ANDROID:
#if ANDROID
                        applies = true;
#endif
                        break;
                    case Q_TABLET:
                        applies = app->_defaultDisplay->sizeClass() == Display::Tablet;
                        break;
                    case Q_MOBILE:
                        applies = app->_defaultDisplay->sizeDiagonalInches() <= 4;
                        break;
                    default:
                        assert(0); // todo: implement
                        break;
                }

                
                if (applies) {
                    // TODO: apply precedence that favours higher specificity
                    newval = &it->second;
                }
            }
            val = newval;
        }
        
        else {
            break;
        }
    }

    return val;
}

const style* style::getValue(const string& keypath) const {
    auto val = this;
    string subkey, key = keypath;
    do {
        val = resolve(val);

        if (!val) {
            return NULL;
        }
        auto dotPos = key.find(".");
        if (dotPos != 0xFFFFFFFF) {
            subkey = key.substr(dotPos+1);// key.substr(dotIndex+1).own();
            key.erase(dotPos);
        } else {
            subkey = "";
        }
        assert(val->type == TypeCompound);
        auto it = val->compound->find(key);
        if (it == val->compound->end()) {
            //log_warn("Value missing for field '%s'", keypath.c_str());
            return NULL;
        }
        val = &it->second;
        key = subkey;
    } while (key.lengthInBytes() > 0);
    return val;
}

static map<string, uint32_t>& s_qualifiers()
{
    static map<string, uint32_t> m({
        {"default", Q_DEFAULT},
        
        {"ios",     Q_IOS},
        {"macos",   Q_MACOS},
        {"android", Q_ANDROID},
        {"web",     Q_WEB},
        {"linux",   Q_LINUX},
        {"windows", Q_WINDOWS},
        {"desktop", Q_DESKTOP},
        {"mobile",  Q_MOBILE},
        {"tablet",  Q_TABLET},
        {"tv",      Q_TV},
        
        {"land",      Q_LANDSCAPE},
        {"landscape", Q_LANDSCAPE},
        {"port",      Q_PORTRAIT},
        {"portrait",  Q_PORTRAIT},
        {"1x",        Q_DPI_1x},
        {"2x",        Q_DPI_2x},
        {"3x",        Q_DPI_3x},
        //lw320
        //sw640

        {"enabled",   MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_DISABLED, 0))},
        {"disabled",  MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_DISABLED, STATE_DISABLED))},
        {"focused",   MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_FOCUSED,STATE_FOCUSED))},
        {"unfocused", MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_FOCUSED,0))},
        {"selected",  MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_SELECTED, STATE_SELECTED))},
        {"unselected",MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_SELECTED, 0))},
        {"pressed",   MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_PRESSED, STATE_PRESSED))},
        {"unpressed", MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_PRESSED, 0))},
        {"checked",   MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_CHECKED, STATE_CHECKED))},
        {"unchecked", MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_CHECKED, 0))},
        {"hover",     MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_HOVER, STATE_HOVER))},
        {"errored",   MAKE_Q(Q_VIEWSTATE, VIEWSTATE(STATE_ERRORED, STATE_ERRORED))},
    });
    return m;
};


static bool parseQual(const string& qualifierName, uint32_t& qual) {
    static auto& qualifiers = s_qualifiers();
    auto it = qualifiers.find(qualifierName);
    if (it != qualifiers.end()) {
        qual = it->second;
        return true;
    }
    bool lw = qualifierName.hasPrefix("lw");
    bool sw = qualifierName.hasPrefix("sw");
    if (lw || sw) {
        qual = MAKE_Q(lw ? Q_SCREEN_WIDTH_LONGER : Q_SCREEN_WIDTH_SHORTER,
                      atoi(qualifierName.c_str()+2));
        return true;
    }
    return false;
}

style::style() : type(TypeSimple), var() {
}
style::style(const style& rval) : type(TypeSimple), var() {
    copyFrom(&rval);
}
style::style(style&& rval) noexcept : type(rval.type), var() {
    switch (rval.type) {
        case TypeSimple: var = rval.var; rval.var.clear(); break;
        case TypeReference: reference = rval.reference; rval.reference = NULL; break;
        case TypeArray: array = rval.array; rval.array = NULL; break;
        case TypeCompound: compound = rval.compound; rval.compound = NULL; break;
        case TypeQual: qualmap = rval.qualmap; rval.qualmap=NULL; break;
    }
    rval.type = TypeSimple;
    rval.var = variant();
}
style::~style() {
    if (type == TypeArray && array) {
        delete array;
    } else if (type == TypeCompound && compound) {
        delete compound;
    } else if (type == TypeQual && qualmap) {
        delete qualmap;
    }
}
void style::setType(enum type newType) {
    if (type == newType) return;
    
    // Handle non-trivial type changes
    bool wasSimple = (type==TypeSimple);
    bool isSimple= (newType==TypeSimple);
    if (wasSimple && !isSimple) {
        var.~variant();
    } else if (!wasSimple && isSimple) {
        new (&var) variant();
    }
    bool wasCompound = (type==TypeCompound);
    bool isCompound = (newType==TypeCompound);
    if (wasCompound && !isCompound) {
        delete compound;
    } else if (!wasCompound && isCompound) {
        compound = new map<string,style>();
    }
    bool wasArray = (type==TypeArray);
    bool isArray = (newType==TypeArray);
    if (wasArray && !isArray) {
        delete array;
    } else if (!wasArray && isArray) {
        array = new vector<style>();
    }
    bool wasQualMap = (type==TypeQual);
    bool isQualMap = (newType==TypeQual);
    if (wasQualMap && !isQualMap) {
        delete qualmap;
    } else if (!wasQualMap && isQualMap) {
        qualmap = new map<uint32_t,style>();
    }

    type = newType;
}

style& style::operator=(const style& other) {
    if (this != &other)  {
        copyFrom(&other);
    }
    return *this;
}
void style::copyFrom(const style* other) {
    setType(other->type);
    switch (other->type) {
        case TypeSimple: var = other->var; break;
        case TypeReference: reference = other->reference; break;
        case TypeArray: array->insert(array->end(), other->array->begin(), other->array->end()); break;
        case TypeCompound: compound->insert(other->compound->begin(), other->compound->end()); break;
        case TypeQual: qualmap->insert(other->qualmap->begin(), other->qualmap->end()); break;
    }
}
bool style::isEmpty() const {
    switch (type) {
        case TypeSimple: return var.isEmpty();
        case TypeReference: return reference==NULL;
        case TypeArray: return array==NULL;
        case TypeCompound: return compound==NULL;
        case TypeQual: return qualmap==NULL;
    }
    return true;
}
bool style::isNumeric() const {
    return (type == TypeSimple) && var.isNumeric();
}
bool style::isFloatingPoint() const {
    return (type == TypeSimple) && var.isFloatingPoint();
}
bool style::isString() const {
    return (type == TypeSimple) && var.isString();
}
bool style::isMeasurement() const {
    return (type == TypeSimple) && var.isMeasurement();
}
bool style::isArray() const {
    return (type == TypeArray);// && s->var.isArray();
}
bool style::isCompound() const {
    return type == TypeCompound;
}

int style::intVal() const {
    if (type==TypeSimple) {
        return var.intVal();
    }
    log_warn("intVal() failed");
    return 0;
}
int style::intVal(const string& name) const {
    auto val = getValue(name);
    if (!val) {
        return 0;
    }
    return val->intVal();
}
bool style::boolVal() const {
    if (type==TypeSimple) {
        return var.boolVal();
    }
    log_warn("boolVal() failed");
    return false;
}

float style::floatVal() const {
    if (type==TypeSimple) {
        return var.floatVal();
    }
    log_warn("floatVal() type coerce failed");
    return 0.f;
}
float style::floatVal(const string& name) const {
    auto val = getValue(name);
    if (!val) {
        return 0;
    }
    return val->floatVal();
}

string style::stringVal() const {
    if (type==TypeSimple) {
        return var.stringVal();
    }
    log_warn("stringVal() type coerce failed");
    return "";
}
string style::stringVal(const string& name) const {
    auto val = getValue(name);
    if (!val) {
        return "";
    }
    return val->stringVal();
}

COLOR style::colorVal(const string& name) const {
    auto v = getValue(name);
    if (v) {
        return v->colorVal();
    }
    return 0;
}

/*int style::intVal(const string& name) const {
    assert(type == Compound);
    auto val2 = compound->find(name);
    if (val2 == compound->end()) {
        log_warn("Value missing for field '%'", name.c_str());
        return 0;
    }
    return val2->second.intVal();
}*/

static vector<style> s_emptyArray;


const vector<style>& style::arrayVal() const {
    if (type==TypeArray) {
        return *array;
    }
    log_warn("arrayVal() type coerce failed");
    return s_emptyArray;
}
const vector<style>& style::arrayVal(const string& name) const {
    auto val = getValue(name);
    if (val) {
        return val->arrayVal();
    }
    return s_emptyArray;
}

EDGEINSETS style::edgeInsetsVal() const {
    EDGEINSETS insets;
    if (isNumeric()) {
        insets.left = insets.top = insets.right = insets.bottom = floatVal();
    } else {
        assert(isArray());
        auto& a = arrayVal();
        if (a.size()==1) {
            insets.left = insets.top = insets.right = insets.bottom = a[0].floatVal();
        } else if (a.size()==2) {
            insets.left = insets.right = a[0].floatVal();
            insets.top = insets.bottom = a[1].floatVal();
        } else if (a.size()==4) {
            insets.left = a[0].floatVal();
            insets.top = a[1].floatVal();
            insets.right = a[2].floatVal();
            insets.bottom = a[3].floatVal();
        } else {
            insets = {0,0,0,0};
            log_warn("Invalid inset, must be 1 or 4 values");
        }
    }
    return insets;
}
EDGEINSETS style::edgeInsetsVal(const string& name) const {
    auto v = getValue(name);
    if (v) {
        return v->edgeInsetsVal();
    }
    return EDGEINSETS_Zero;
}

float style::fontWeightVal(const string& name) const {
    auto v = getValue(name);
    if (v) {
        return v->fontWeightVal();
    }
    return 0;
}

const variant& style::variantVal() const {
    assert(type == TypeSimple);
    return var;
}

COLOR style::colorVal()  const {
    if (isNumeric()) {
        if (isFloatingPoint()) {
            uint8_t lum = floatVal() * 255;
            return 0xFF000000 | (lum<<16) | (lum<<8) | lum;
        } else {
            return intVal();
        }
    } else if (isString()) {
        // TODO: make this method non-const and have it mutate type to Int after the colour is parsed
        string colorVal = stringVal();
        // value is in either '#fff', '#ffffff', '#ffffffff',
        if (colorVal.hadPrefix("#")) {
            unsigned int val = (unsigned int)strtoul(colorVal.c_str(), NULL, 16);
            if (colorVal.length() == 3) {
                int r = (val & 0xF00)>>8;
                int g = (val & 0xF0)>>4;
                int b = val & 0xF;
                return 0xFF000000
                | (((b<<4)|b)<<16)
                | (((g<<4)|g)<<8)
                | ((r<<4)|r);
            }
            else if (colorVal.length()==6) {
                uint32_t r = (val & 0xFF0000)>>16;
                uint32_t g = (val & 0xFF00)>>8;
                uint32_t b = val & 0xFF;
                return 0xFF000000
                | (b<<16)
                | (g<<8)
                | (r);
            }
            else if (colorVal.length()==8) {
                uint32_t r = (val & 0xFF0000)>>16;
                uint32_t g = (val & 0xFF00)>>8;
                uint32_t b = val & 0xFF;
                return (val & 0xFF000000)
                | (b<<16)
                | (g<<8)
                | (r);
            }
            else {
                log_warn("Malformed hex color value");
                return 0;
            }
        }
        // 'rgb(255,255,255)' or 'rgba(255,255,255,255) notation
        else if (colorVal.hadPrefix("rgb(")) {
            //
        }
        // Web color names
        else {
            static map<string, COLOR> htmlColors = {
                // Pinks
                {"pink", 0xffffc0cb},
                {"lightpink", 0xffffb6c1},
                {"hotpink", 0xffff69b4},
                {"deeppink", 0xffff1493},
                {"palevioletred", 0xffdb7093},
                {"mediumvioletred", 0xffc71585},
                // reds
                {"lightsalmon", 0xffffa07a},
                {"salmon", 0xfffa8072},
                {"darksalmon", 0xffe9967a},
                {"lightcoral", 0xfff08080},
                {"indianred", 0xffcd5c5c},
                {"crimson", 0xffdc143c},
                {"firebrick", 0xffb22222},
                {"darkred", 0xff8b0000},
                {"red", 0xffff0000},
                // oranges
                {"orangered", 0xffff4500},
                {"tomato", 0xffff6347},
                {"coral", 0xffff7f50},
                {"darkorange", 0xffff8c00},
                {"orange", 0xffffa500},
                // yellows
                {"yellow", 0xffffff00},
                {"lightyellow", 0xffffffe0},
                {"lemonchiffon", 0xfffffacd},
                {"lightgoldenrodyellow", 0xfffafad2},
                {"papayawhip", 0xffffefd5},
                {"moccasin", 0xffffe4b5},
                {"peachpuff", 0xffffdab9},
                {"palegoldenrod", 0xffeee8aa},
                {"khaki", 0xfff0e68c},
                {"darkkhaki", 0xffbdb76b},
                {"gold", 0xffffd700},
                // browns
                {"cornsilk", 0xfffff8dc},
                {"blanchedalmond", 0xffffebcd},
                {"bisque", 0xffffe4c4},
                {"navajowhite", 0xffffdead},
                {"wheat", 0xfff5deb3},
                {"burlywood", 0xffdeb887},
                {"tan", 0xffd2b48c},
                {"rosybrown", 0xffbc8f8f},
                {"sandybrown", 0xfff4a460},
                {"goldenrod", 0xffdaa520},
                {"darkgoldenrod", 0xffb8860b},
                {"peru", 0xffcd853f},
                {"chocolate", 0xffd2691e},
                {"saddlebrown", 0xff8b4513},
                {"sienna", 0xffa0522d},
                {"brown", 0xffa52a2a},
                {"maroon", 0xff800000},
                // greens
                {"darkolivegreen", 0xff556b2f},
                {"olive", 0xff808000},
                {"olivedrab", 0xff6b8e23},
                {"yellowgreen", 0xff9acd32},
                {"limegreen", 0xff32cd32},
                {"lime", 0xff00ff00},
                {"lawngreen", 0xff7cfc00},
                {"chartreuse", 0xff7fff00},
                {"greenyellow", 0xffadff2f},
                {"springgreen", 0xff00ff7f},
                {"mediumspringgreen", 0xff00fa9a},
                {"lightgreen", 0xff90ee90},
                {"palegreen", 0xff98fb98},
                {"darkseagreen", 0xff8fbc8f},
                {"mediumaquamarine", 0xff66cdaa},
                {"mediumseagreen", 0xff3cb371},
                {"seagreen", 0xff2e8b57},
                {"forestgreen", 0xff228b22},
                {"green", 0xff008000},
                {"darkgreen", 0xff006400},
                // cyans
                {"aqua", 0xff00ffff},
                {"cyan", 0xff00ffff},
                {"lightcyan", 0xffe0ffff},
                {"paleturquoise", 0xffafeeee},
                {"aquamarine", 0xff7fffd4},
                {"turquoise", 0xff40e0d0},
                {"mediumturquoise", 0xff48d1cc},
                {"darkturquoise", 0xff00ced1},
                {"lightseagreen", 0xff20b2aa},
                {"cadetblue", 0xff5f9ea0},
                {"darkcyan", 0xff008b8b},
                {"teal", 0xff008080},
                // blues
                {"lightsteelblue", 0xffb0c4de},
                {"powderblue", 0xffb0e0e6},
                {"lightblue", 0xffadd8e6},
                {"skyblue", 0xff87ceeb},
                {"lightskyblue", 0xff87cefa},
                {"deepskyblue", 0xff00bfff},
                {"dodgerblue", 0xff1e90ff},
                {"cornflowerblue", 0xff6495ed},
                {"steelblue", 0xff4682b4},
                {"royalblue", 0xff4169e1},
                {"blue", 0xff0000ff},
                {"mediumblue", 0xff0000cd},
                {"darkblue", 0xff00008b},
                {"navy", 0xff000080},
                {"midnightblue", 0xff191970},
                // purples
                {"lavender", 0xffe6e6fa},
                {"thistle", 0xffd8bfd8},
                {"plum", 0xffdda0dd},
                {"violet", 0xffee82ee},
                {"orchid", 0xffda70d6},
                {"fuchsia", 0xffff00ff},
                {"magenta", 0xffff00ff},
                {"mediumorchid", 0xffba55d3},
                {"mediumpurple", 0xff9370db},
                {"blueviolet", 0xff8a2be2},
                {"darkviolet", 0xff9400d3},
                {"darkorchid", 0xff9932cc},
                {"darkmagenta", 0xff8b008b},
                {"purple", 0xff800080},
                {"indigo", 0xff4b0082},
                {"darkslateblue", 0xff483d8b},
                {"slateblue", 0xff6a5acd},
                {"mediumslateblue", 0xff7b68ee},
                // whites
                {"white", 0xffffffff},
                {"snow", 0xfffffafa},
                {"honeydew", 0xfff0fff0},
                {"mintcream", 0xfff5fffa},
                {"azure", 0xfff0ffff},
                {"aliceblue", 0xfff0f8ff},
                {"ghostwhite", 0xfff8f8ff},
                {"whitesmoke", 0xfff5f5f5},
                {"seashell", 0xfffff5ee},
                {"beige", 0xfff5f5dc},
                {"oldlace", 0xfffdf5e6},
                {"floralwhite", 0xfffffaf0},
                {"ivory", 0xfffffff0},
                {"antiquewhite", 0xfffaebd7},
                {"linen", 0xfffaf0e6},
                {"lavenderblush", 0xfffff0f5},
                {"mistyrose", 0xffffe4e1},
                // grays and black
                {"gainsboro", 0xffdcdcdc},
                {"lightgray", 0xffd3d3d3},
                {"silver", 0xffc0c0c0},
                {"darkgray", 0xffa9a9a9},
                {"gray", 0xff808080},
                {"dimgray", 0xff696969},
                {"lightslategray", 0xff778899},
                {"slategray", 0xff708090},
                {"darkslategray", 0xff2f4f4f},
                {"black", 0xff000000}
            };
            auto it = htmlColors.find(colorVal.lowercase());
            if (it != htmlColors.end()) {
                uint32_t argb = it->second;
                uint32_t r = (argb & 0xFF0000)>>16;
                uint32_t g = (argb & 0xFF00)>>8;
                uint32_t b = argb & 0xFF;
                return (argb & 0xFF000000) | (b<<16) | (g<<8) | (r);
            }
        }
        
    }
    log_warn("colorVal() failed");
    return 0;
}

/*
float StyleValue::floatVal(const string& name) const {
    auto field = getValue(name);
    if (!field) {
        log_warn("Value missing for field '%'", name.c_str());
        return 0;
    }
    return field->floatVal();
}
measurement StyleValue::measurementVal() const {
    assert(type == Measure);
    return _measurement;
}

static map<string, StyleValue> s_emptyMap;

const map<string, StyleValue>& StyleValue::compoundVal() const {
    auto val = select();
    if (val->type==Type::Compound) return *(val->compound);
    log_warn("compoundVal() type coerce failed");
    return s_emptyMap;
}
*/


measurement style::measurementVal() const {
    if (type == TypeSimple && var.isMeasurement()) {
        return var.measurementVal();
    }
    return measurement(0,measurement::PX);
}


VECTOR4 style::cornerRadiiVal() const {
    VECTOR4 r;
    if (isNumeric()) {
        r.x = r.y = r.z = r.w = floatVal();
    } else {
        assert(isArray());
        auto& radii = arrayVal();
        if (radii.size()==1) {
            r.x = r.y = r.z = r.w = radii[0].floatVal();
        } else if (radii.size()==4) {
            r.x = radii[0].floatVal();
            r.y = radii[1].floatVal();
            r.z = radii[2].floatVal();
            r.w = radii[3].floatVal();
        } else {
            log_warn("Invalid corner-radii, must be 1 or 4 values");
            r = {0,0,0,0};
        }
    }
    return r;
}


float style::fontWeightVal() const {
    if (isNumeric()) {
        return floatVal();
    } else if (isString()) {
        string fw = stringVal().lowercase();
        if (fw=="bold") return FONT_WEIGHT_BOLD;
        if (fw=="regular") return FONT_WEIGHT_REGULAR;
        if (fw=="medium") return FONT_WEIGHT_MEDIUM;
        if (fw=="semibold") return FONT_WEIGHT_SEMIBOLD;
        if (fw=="light") return FONT_WEIGHT_LIGHT;
        if (fw=="thin") return FONT_WEIGHT_THIN;
        if (fw=="ultralight") return FONT_WEIGHT_ULTRA_LIGHT;
        if (fw=="heavy") return FONT_WEIGHT_HEAVY;
        if (fw=="black") return FONT_WEIGHT_BLACK;
    }
    log_warn("Invalid fontWeight");
    return 0;
}





void style::importNamedValues(const map<string,style>& styleValues) {
    if (isEmpty()) {
        type = TypeCompound;
        compound = new map<string, style>();
    }
    assert(isCompound());
    for (auto v=styleValues.begin() ; v!=styleValues.end() ; v++) {
        const string& name = v->first;
        const style& val = v->second;
        if (val.type == TypeCompound) {
            const auto& e = compound->find(name);
            if (e == compound->end()) {
                (*compound)[name] = val;
            } else {
                auto& eval = e->second;
                if (eval.type == TypeCompound) {
                    eval.importNamedValues(*val.compound);
                } else {
                    (*compound)[name] = val;
                }
            }
        } else {
            compound->insert(make_pair(name, val));
            (*compound)[name] = val;
        }
    }
}

bool style::parse(const string& str) {
    variant v = variant::parse(str, 0);
    if (v.isEmpty()) {
        return false;
    }
    fromVariant(v);
    return true;
}

void style::fromVariant(const variant& v) {

    // Expand compounds
    if (v.isCompound()) {
        setType(TypeCompound);
        
        // Iterate the compound's fields
        auto compoundVal = v.compoundRef();
        for (auto e : compoundVal) {
            
            // Get the field value
            style styleVal;
            styleVal.fromVariant(e.second);

            // See if field name includes a qualifier. If not, add
            // the new value to the compound and continue
            auto qualifierStart = e.first.find("@");
            if (qualifierStart>=e.first.lengthInBytes()) {
                compound->insert(make_pair(e.first, styleVal));
                continue;
            }

            // Extract the qualifier name and convert it to a struct qual (a uint32_t)
            string qualifierName = e.first.substr(qualifierStart+1);
            uint32_t qual;
            if (!parseQual(qualifierName, qual)) {
                log_error("Invalid qualifier: %s", qualifierName.c_str());
                return;
            }

            // If qualifier is pure (i.e. no field name) then we can only be parsing a qualifier map
            // and all we have to do is add the new style value to it
            bool isPure = qualifierStart==0;
            if (isPure) {
                if (type == TypeCompound) {
                    if (compound->size() > 0) {
                        log_error("Cannot mix pure and non-pure qualified values");
                        return;
                    }
                    setType(TypeQual);
                }
                qualmap->insert(make_pair(qual, styleVal));
                continue;
            }
                
            // Handle a qualified field name. First we look up any existing value.
            string fieldName = e.first.substr(0, qualifierStart);
            auto existingVal = compound->find(fieldName);
            
            // If there is an existing value and it's not already a qualifier map, then create
            // a new qualmap value with the default entry set to the existing value.
            if (existingVal != compound->end()) {
                if (existingVal->second.type != TypeQual) {
                    style newVal;
                    newVal.setType(TypeQual);
                    newVal.qualmap->insert(make_pair(0, existingVal->second)); // 0 == @default
                    existingVal->second = newVal;
                }
                existingVal->second.qualmap->insert(make_pair(qual, styleVal));
                continue;
            }
            
            // No existing value, so create a new qualmap
            style newVal;
            newVal.setType(TypeQual);
            newVal.qualmap->insert(make_pair(qual, styleVal));
            compound->insert(make_pair(fieldName, newVal));
        }
        
    }
    
    else if (v.isArray()) {
        setType(TypeArray);
        for (auto& e : v.arrayRef()) {
            style styleVal;
            styleVal.fromVariant(e);
            array->emplace_back(styleVal);
        }

    }
    
    else {
        setType(TypeSimple);
        var = v;
    }

}



void Styleable::applyStyle(const string& name) {
    auto value = s_styleRoot.get(name);
    if (value) {
        applyStyle(*value);
    }
}

void Styleable::applyStyle(const style& astyle) {
    const style* style = style::resolve(&astyle);

    // TODO: handle single values and arrays

    assert(style->isCompound());
    
    // Ensure the 'style' attribute, if present, gets processed first, because the others may override it
    for (auto& field : *style->compound) {
        if (field.first == "style") {
            applyStyle(field.second);
            break;
        }
    }
    
    for (auto& field : *style->compound) {
        if (field.first == "style") continue;
        const class style* val = &field.second;
        val = style::resolve(val);
        if (val->type == style::TypeQual) {
            if (!_dynamicStyles) {
                _dynamicStyles = new map<string, const class style*>();
            }
            (*_dynamicStyles)[field.first] = val;
            val = resolveQualifiedStyle(val);
        } else {
            if (_dynamicStyles) {
                auto it = _dynamicStyles->find(field.first);
                if (it != _dynamicStyles->end()) {
                    _dynamicStyles->erase(it);
                }
                if (!_dynamicStyles->size()) {
                    delete _dynamicStyles;
                    _dynamicStyles = nullptr;
                }
            }

        }
        if (!applySingleStyle(field.first, *val)) {
            log_warn("Ignored unknown attribute '%s'", field.first.c_str());
        }
    }
}

const style* Styleable::resolveQualifiedStyle(const style* style) {
    assert(style->type == style::TypeQual);
    auto it = style->qualmap->find(Q_DEFAULT);
    if (it == style->qualmap->end()) {
        return nullptr;
    }
    return style::resolve(&it->second);
}

const style* Styleable::getStyle(const string& key) {
    return style::get(key);
}

float Styleable::getStyleFloat(const string& key) {
    auto value = getStyle(key);
    if (!value) {
        log_warn("Missing float style info '%s'", key.c_str());
        return 0;
    }
    return value->floatVal();
}
COLOR Styleable::getStyleColor(const string& key) {
    auto value = getStyle(key);
    if (!value) {
        log_warn("Missing color style info '%s'", key.c_str());
        return 0;
    }
    return value->colorVal();

}
