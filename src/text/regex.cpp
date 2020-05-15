//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



RegEx::RegEx(const string& pattern) {
    uint32_t p = 0;
    int currentGroup = -1;
    int numCapturedGroups = 0;
    while (p<pattern.lengthInBytes()) {
        char32_t c = pattern.readChar(p);
        // Start of group
        if (c=='(') {
            //_groups.emplace_back(Group {(int)_pattern.size(), -1});
            currentGroup = numCapturedGroups++;
            continue;
        }
        // End of group
        if (c==')') {
            //_groups.rbegin()->end = (int)_pattern.size();
            currentGroup = -1;
            continue;
        }
        // New token
        Token t;
        t.quantMin = t.quantMax = 0;
        t.inverted = false;
        t.group = currentGroup;
        // Parse character class
        if (c=='.') {
            t.charClass = Any;
        } else if (c=='\\') {
            char32_t esc =  pattern.readChar(p);
            if (esc == 'w') t.charClass = Word;
            else if (esc == 'W') {t.charClass = Word; t.inverted = true;}
            else if (esc == 'd') t.charClass = Digit;
            else if (esc == 'D') {t.charClass = Digit; t.inverted = true;}
            else if (esc == 's') t.charClass = Whitespace;
            else if (esc == 'S') {t.charClass = Whitespace; t.inverted = true;}
            else if (esc == '+' || esc=='*' || esc=='?' || esc=='^' || esc=='$' || esc=='\\' || esc=='.' || esc=='[' || esc==']' || esc=='{' || esc=='}' || esc=='(' || esc==')' || esc=='|' || esc=='/' || esc==')') {t.charClass=Range; t.chars.push_back(esc);}
            else log_warn("ignoring unknown escape");
        } else if (c=='[') {
            if (pattern.peekChar(p)=='^') {
                t.inverted = true;
                pattern.readChar(p);
            }
            while (p<pattern.lengthInBytes()) {
                c = pattern.readChar(p);
                if (c==']') {
                    break;
                }
                t.chars.push_back(c);
                if (pattern.peekChar(p) == '-') { // range
                    assert(0); // todo
                }
            }
        } else {
            t.charClass = Range;
            t.chars.push_back(c);
        }
        // Quantifiers
        c = pattern.peekChar(p);
        if (c=='?') {
            t.quantMin = 0;
            t.quantMax = 1;
            pattern.readChar(p);
        } else if (c=='+') {
            t.quantMin = 1;
            t.quantMax = INT32_MAX;
            pattern.readChar(p);
        } else if (c=='*') {
            t.quantMin = 0;
            t.quantMax = INT32_MAX;
            pattern.readChar(p);
        } else if (c=='{') {
            pattern.readChar(p);
            t.quantMin = pattern.readNumber(p).asInt();
            c = pattern.peekChar(p);
            if (c == ',') {
                pattern.readChar(p);
                if (pattern.peekChar(p)=='}') {
                    t.quantMax = INT32_MAX;
                } else {
                    t.quantMax = pattern.readNumber(p).asInt();
                    assert(t.quantMax>=t.quantMin);
                }
            } else {
                t.quantMax = t.quantMin;
            }
            c = pattern.readChar(p);
            assert(c == '}');
        } else {
            t.quantMin = 1;
            t.quantMax = 1;
        }
        _pattern.push_back(t);
    }
}
    

RegEx::MatchResult RegEx::match(const string& str, vector<string>& matchedGroups) {
    matchedGroups.clear();
    if (str.length()==0) {
        return MatchPartial;
    }
    uint32_t it = 0;
    int tokenIndex = -1;
    for (auto token : _pattern) {
        tokenIndex++;
        if (it>=str.lengthInBytes()) {
            return MatchPartial;
        }
        int q = 0;
        while (q<token.quantMax) {
            bool match = false;
            char32_t ch = str.peekChar(it);
            switch (token.charClass) {
                case Range:
                    for (auto c : token.chars) {
                        if (c == ch) {
                            match = true;
                            break;
                        }
                    }
                    break;
                case Any:
                    match = true;
                    break;
                case Word:
                    match = (ch>='A'&&ch<='Z') || (ch>='a'&&ch<='z') || ch=='_';
                    break;
                case Digit:
                    match = (ch>='0'&&ch<='9');
                    break;
                case Whitespace:
                    match = (ch==' '||ch=='\r'||ch=='\n'||ch=='\t');
                    break;
            }
            if (token.inverted) {
                match = !match;
            }
            if (!match) {
                break;
            }
            
            // Char matched the pattern
            q++;
            str.readChar(it);
            
            // Add matched char to current group
            if (token.group >= 0) {
                while (matchedGroups.size() <= token.group) {
                    matchedGroups.push_back(string());
                }
                matchedGroups.at(token.group).append(ch);
            }
        }
        if (q < token.quantMin) {
            return q?MatchPartial:MatchNone;
        }
    }
    return (it>=str.lengthInBytes()) ? MatchFull : MatchNone;
}

    
