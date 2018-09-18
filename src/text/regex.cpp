//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



RegEx::RegEx(const string& pattern) {
    StringProcessor p(pattern);
    int currentGroup = -1;
    int numCapturedGroups = 0;
    while (!p.eof()) {
        char32_t c = p.next();
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
            char32_t esc = p.next();
            if (esc == 'w') t.charClass = Word;
            else if (esc == 'W') {t.charClass = Word; t.inverted = true;}
            else if (esc == 'd') t.charClass = Digit;
            else if (esc == 'D') {t.charClass = Digit; t.inverted = true;}
            else if (esc == 's') t.charClass = Whitespace;
            else if (esc == 'S') {t.charClass = Whitespace; t.inverted = true;}
            else if (esc == '+' || esc=='*' || esc=='?' || esc=='^' || esc=='$' || esc=='\\' || esc=='.' || esc=='[' || esc==']' || esc=='{' || esc=='}' || esc=='(' || esc==')' || esc=='|' || esc=='/' || esc==')') {t.charClass=Range; t.chars.push_back(esc);}
            else app.warn("ignoring unknown escape");
        } else if (c=='[') {
            if (p.peek()=='^') {
                t.inverted = true;
                p.next();
            }
            while (!p.eof()) {
                c = p.next();
                if (c==']') {
                    break;
                }
                t.chars.push_back(c);
                if (p.peek() == '-') { // range
                    assert(0); // todo
                }
            }
        } else {
            t.charClass = Range;
            t.chars.push_back(c);
        }
        // Quantifiers
        c = p.peek();
        if (c=='?') {
            t.quantMin = 0;
            t.quantMax = 1;
            p.next();
        } else if (c=='+') {
            t.quantMin = 1;
            t.quantMax = INT32_MAX;
            p.next();
        } else if (c=='*') {
            t.quantMin = 0;
            t.quantMax = INT32_MAX;
            p.next();
        } else if (c=='{') {
            p.next();
            t.quantMin = p.nextNumber().asInt();
            c = p.peek();
            if (c == ',') {
                p.next();
                if (p.peek()=='}') {
                    t.quantMax = INT32_MAX;
                } else {
                    t.quantMax = p.nextNumber().asInt();
                    assert(t.quantMax>=t.quantMin);
                }
            } else {
                t.quantMax = t.quantMin;
            }
            c = p.next();
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
    StringProcessor it(str);
    int tokenIndex = -1;
    for (auto token : _pattern) {
        tokenIndex++;
        if (it.eof()) {
            return MatchPartial;
        }
        int q = 0;
        while (q<token.quantMax) {
            bool match = false;
            char32_t ch = it.peek();
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
            it.next();
            
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
    return it.eof() ? MatchFull : MatchNone;
}

    
