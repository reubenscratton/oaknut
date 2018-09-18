//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 A very simple regex that can handle partial matches. Far from perfect but good enough
 for phone numbers and email addresses.
 
 TODO: Alternator support
 TODO: Allow quantifiers on groups
 TODO: Allow non-capturing groups (?:)
 TODO: anchors
 TODO: references
 */

class RegEx : public Object {
public:
    
    enum CharClass {
        Range, Any, Word, Digit, Whitespace
    };
    
    struct Token {
        CharClass charClass;
        vector<char32_t> chars;
        bool inverted;
        int quantMin, quantMax;
        int group;
    };
    vector<Token> _pattern;
    
    RegEx(const string& pattern);
    
    enum MatchResult {
        MatchNone,
        MatchPartial,
        MatchFull
    };
    
    MatchResult match(const string& str, vector<string>& matchedGroups);
    
private:
};
