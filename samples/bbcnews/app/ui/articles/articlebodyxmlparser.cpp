//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "articlebodyxmlparser.h"
#include "elementtext.h"
#include "elementmedia.h"
//#import "BNTextTraits.h"

/**
 
 Articles are rendered as a flat series of paragraphs (i.e. text elements).
 
 Article body XML is a bit ambiguous as to where paragraphs begin and end. Most obviously
 we have the <paragraph> tag, but at the top level we can also have <list> which *implies*
 a paragraph container (i.e. the list has paragraph spacing above and below). Other
 paragraph-implicit tags are <heading>, <subheading>, and <crosshead>.
 
 Similarly some top-level tags do not imply a paragraph but merely act as styling for
 one or more <paragraph> tags contained within it. Examples are <question> and <answer>.
 
 There is no support for 'table', 'comment', or 'pull-out' tags at this time.
 
 */

/*
static bool xmlParse(const string& xml, std::function<void(const string&,const map<string,string>&)> onElementStart) {
    string::ptr it = xml.begin();
    int elementDepth = 0;
    while (!it.eof()) {
        it.skipWhitespace();
        char32_t ch = it.next();
        if (ch!='<') {
            app->warn("Expected '<'");
        }
        string tag = it.readIdentifier();
        map<string, string> attribs;
        for (;;) {
            it.skipWhitespace();
            if (it.nextWas(">")) {
                break;
            }
            if (it.eof()) {
                app->warn("Expected '>'");
                return false;
            }
            string attributeName = it.readIdentifier();
            if (it.nextWas(":")) {
                attributeName += it.readIdentifier();
            }
            it.skipWhitespace();
            if (!it.nextWas("=")) {
                app->warn("Expected '='");
                return false;
            }
            it.skipWhitespace();
            if (!it.nextWas("\"")) {
                app->warn("Expected '\"'");
                return false;
            }
            attribs[attributeName] = it.readUpTo("\"");
            it.next();
        }
        onElementStart(tag, attribs);

        elementDepth++;
    }
    return true;
}
*/

/**
 A non-validating streaming parser, a good choice if you don't need or want the whole DOM in memory but need to extract a custom
 data representation from a single sweep of the XML data.

currentTag() - returns the name of the current tag, or an empty string if there isn't one (empty or malformed doc).
attributeCount() - returns the number of attributes on the current tag
attributeValue(name) - returns the value of the named attribute, or an empty string if it doesnt exist
attributeValue(index) - returns the value of the indexed attribute, or an empty string if out of bounds
string nextTag() - moves to the next tag and returns the non-tag content leading up to it.
void toClosingTag() - moves to the closing tag corresponding to the current tag. Does nothing if current tag is a closing tag.

 */

class XmlParser : public Object {
public:
    XmlParser(const string& str);
    
    // bool eof() { return _ptr != string::eof; }
    const string& currentTag() const { return _currentTag; }
    string nextTag();
    string attributeValue(const string& name);
    
    XmlParser currentTagContents() {
        auto start = _ptr;
        auto openingTag = currentTag();
        string closingTag = "/"+openingTag;
        while (currentTag() != closingTag) {
            nextTag();
        }
        auto end = _ptr;
        nextTag();
        return XmlParser(_str.substr(start, end));
    }
    
private:
    const string& _str;
    uint32_t _ptr;
    string _currentTag, _currentAttribsStr;
    map<string, string> _currentAttribs;
    
    string readAttributeName(const string& s, uint32_t& o) {
        s.skipWhitespace(o);
        auto start = o;
        while (o < s.lengthInBytes()) {
            char32_t ch = s.readChar(o);
            if ((ch>='A'&&ch<='Z')
             || (ch>='a'&&ch<='z')
             || (ch>='0'&&ch<='9')
             || ch=='_' || ch=='-' || ch==':') {
                continue;
            }
            s.rewind(o);
            break;
        }
        return string(s, start, o);
    }
    
    string readQuotedString(const string& s, uint32_t& o) {
        char quoteChar = '\0';
        if (s.skipChar(o, '\"')) quoteChar='\"';
        else if (s.skipChar(o, '\'')) quoteChar='\'';
        
        string q = s.readUpTo(o, quoteChar);
        s.readChar(o); // todo: need a readPast() api...
        
        uint32_t qo=0;
        string r;
        while (qo < q.lengthInBytes()) {
            r += q.readUpTo(qo, "&");
            if (qo < q.lengthInBytes()) {
                string code = q.readUpTo(qo, ";");
                if (code == "lt") r+="<";
                else if (code == "gt") r+=">";
                else if (code == "quot") r+="\"";
                else if (code == "apos") r+="\'";
                else if (code == "amp") r+="&";
                else assert(0); //todo: support char encoding &#60;
            }
        }
        return r;
    }
};

XmlParser::XmlParser(const string& str) : _str(str), _ptr(0), _currentTag() {
    nextTag();
}
string XmlParser::nextTag() {
    
    // Extract content up to the opening bracket
    string content = _str.readUpTo(_ptr, "<");
    if (_ptr < _str.lengthInBytes()) {
        _ptr++;
        if (_str.peekChar(_ptr)=='/') {
            _currentTag = _str.readUpTo(_ptr, ">");
        } else {
            _currentTag = _str.readUpToOneOf(_ptr, "> \t\r\n/");
            _currentAttribsStr = _str.readUpTo(_ptr, ">");
            _currentAttribs.clear();
        }
        _ptr++;
    }
    return content;
}

string XmlParser::attributeValue(const string& name) {
    if (_currentAttribsStr.lengthInBytes()) {
        uint32_t o=0;
        while (o<_currentAttribsStr.lengthInBytes()) {
            _currentAttribsStr.skipWhitespace(o);
            string attribName = readAttributeName(_currentAttribsStr, o);
            if (!attribName.lengthInBytes()) {
                break;
            }
            _currentAttribsStr.skipWhitespace(o);
            string attribValue;
            if (_currentAttribsStr.skipChar(o, '=')) {
                _currentAttribsStr.skipWhitespace(o);
                attribValue = readQuotedString(_currentAttribsStr, o);
            }
            _currentAttribs[attribName] = attribValue;
        }
        _currentAttribsStr.clear();
    }
    return _currentAttribs[name];
}


BNArticleBodyXmlParser::BNArticleBodyXmlParser(const string& str, BNItem* item) {
    
    XmlParser xml(str);
    if (xml.currentTag() == "body") {
        while (xml.currentTag() != "/body") {
            
            // Text elements
            if (xml.currentTag().isOneOf({"paragraph", "heading", "subheading", "crosshead", "list" })) {
                string styleName = xml.currentTag();
                if (styleName == "paragraph") {
                    styleName = xml.attributeValue("role");
                }
                XmlParser xmlPara = xml.currentTagContents();
            }
            
            // Images and videos
            else if (xml.currentTag() == "image" || xml.currentTag() == "video") {
                string id = xml.attributeValue("id");
                xml.nextTag();
            }
            
            // Anything else, ignore.
            else {
                xml.nextTag();
            }
        }
    }
        
}

/*
@interface BNArticleBodyXmlParser ()
@property (nonatomic) NSString* currentStyle;
@property (nonatomic) NSMutableArray* styleStack;
@property (nonatomic) NSMutableArray* elementStack;
@property (nonatomic) NSMutableArray* traitStack;
@property (nonatomic) BOOL listIsOrdered;
@property (nonatomic) NSInteger listOrdinal;
@property (nonatomic) BOOL skippingElements;
@property (nonatomic, weak) BNItem* item;
@property (nonatomic) NSString* currentElementName;
@end


@implementation BNArticleBodyXmlParser

- (id)initWithData:(NSData *)data andItem:(BNItem*)item {
	self = [super initWithData:data];
	if (self)
	{
		self.delegate = self;
		self.elementStack = [[NSMutableArray alloc] init];
		self.traitStack = [[NSMutableArray alloc] init];
		self.styleStack = [[NSMutableArray alloc] init];
		self.item = item;
	}
	return self;
}

- (void)pushStyle:(NSString*)newStyle {
	if (self.currentStyle != nil) {
		[self.styleStack addObject:self.currentStyle];
	}
	self.currentStyle = newStyle;
}
- (void)popStyle {
	if (self.styleStack.count == 0) {
		self.currentStyle = nil;
	}
	else {
		self.currentStyle = self.styleStack.lastObject;
		[self.styleStack removeLastObject];
	}
}
- (NSString*)safeStyle : (NSString*)style {
	if (style == nil) {
		style = self.currentStyle;
	}
	if (style == nil) {
		style = @"normal";
	}
	return style;
}

- (void)beginTopLevelElement : (id)element {
	self.currentElement = element;
	[self.item.elements addObject:self.currentElement];
}

- (void)parser:(NSXMLParser *)parser
didStartElement:(NSString *)elementName
  namespaceURI:(NSString *)namespaceURI
 qualifiedName:(NSString *)qualifiedName
	attributes:(NSDictionary *)attributeDict {
	
	if ([elementName isEqualToString:@"body"]) {
		return;
	}
	
	self.elementDepth++;
	
	if (self.skippingElements) {
		return;
	}
	
	// Style-only
	self.currentElementName = elementName;
	if ([elementName isEqualToString:@"question"]) {
		[self pushStyle:@"question"];
		return;
	} else if ([elementName isEqualToString:@"answer"]) {
		[self pushStyle:@"answer"];
		return;
	}
	
	// New paragraph
	if ([elementName isEqualToString:@"paragraph"]) {
		[self beginTopLevelElement : [[BNElementText alloc] initWithStyle:[self safeStyle:attributeDict[@"role"]]]];
	} else if ([elementName isEqualToString:@"heading"]) {
		[self beginTopLevelElement : [[BNElementText alloc] initWithStyle:@"heading"]];
	} else if ([elementName isEqualToString:@"subheading"]) {
		[self beginTopLevelElement : [[BNElementText alloc] initWithStyle:@"subheading"]];
	} else if ([elementName isEqualToString:@"crosshead"]) {
		[self beginTopLevelElement : [[BNElementText alloc] initWithStyle:@"crosshead"]];
	} else if ([elementName isEqualToString:@"list"]) {
		[self beginTopLevelElement : [[BNElementText alloc] initWithStyle:@"list"]];
		self.listIsOrdered = [attributeDict[@"type"] isEqualToString:@"ordered"];
		self.listOrdinal = 1;
		
		// Non-text top-level elements
	} else if ([elementName isEqualToString:@"image"]) {
		BNElementImage* element = [[BNElementImage alloc] initWithRelationship:[self.item imageForMediaId:attributeDict[@"id"]]];
		[self beginTopLevelElement : element];
	} else if ([elementName isEqualToString:@"video"]) {
		BNElementVideo* element = [[BNElementVideo alloc] initWithRelationship:[self.item videoForMediaId:attributeDict[@"id"]]];
		[self beginTopLevelElement : element];
	} else if ([elementName isEqualToString:@"audio"]) {
		BNElementAudio* element = [[BNElementAudio alloc] initWithRelationship:[self.item audioForMediaId:attributeDict[@"id"]]];
		[self beginTopLevelElement : element];
	}
	
	
	else {
		
		// Unsupported top-level element: <table>, <pull-out>, etc. Make sure we don't process their child elements.
		if (self.elementDepth == 1) {
			self.skippingElements = YES;
		}
		
		// Element contents
		if (self.currentElement != nil) {
			
			// Text traits (all we support at present)
			if ([self.currentElement isKindOfClass:[BNElementText class]]) {
				BNElementText* textElement = (BNElementText*)self.currentElement;
				if ([elementName isEqualToString:@"bold"]) {
					BNTextTraitBold* trait = [[BNTextTraitBold alloc] initWithStart:textElement.text.length];
					[textElement.traits addObject:trait];
					[self.traitStack addObject:trait];
				} else if ([elementName isEqualToString:@"italic"]) {
					BNTextTraitItalic* trait = [[BNTextTraitItalic alloc] initWithStart:textElement.text.length];
					[textElement.traits addObject:trait];
					[self.traitStack addObject:trait];
				} else if ([elementName isEqualToString:@"listItem"]) {
					[textElement trimTrailingWhitespace];
					NSString* itemPrefix = nil;
					if (self.listIsOrdered) {
						itemPrefix = [NSString stringWithFormat:@"%lu.  ", (unsigned long)self.listOrdinal++];
					} else {
						itemPrefix = @"\u25A0   ";
						BNTextTraitForecolor* greyColor = [[BNTextTraitForecolor alloc] initWithStart:textElement.text.length];
						greyColor.forecolor = [UIColor lightGrayColor];
						greyColor.end = greyColor.start + 1;//itemPrefix.length;
						[textElement.traits addObject:greyColor];
						BNTextTraitFontScale* shrink = [[BNTextTraitFontScale alloc] initWithStart:textElement.text.length];
						shrink.scale = 0.5f;
						shrink.end = greyColor.end;
						[textElement.traits addObject:shrink];
						BNTextTraitBaselineOffset* offset = [[BNTextTraitBaselineOffset alloc] initWithStartAndEnd:shrink.start end:shrink.end];
						offset.distance = 0.3f;
						[textElement.traits addObject:offset];
					}
					textElement.headIndentNumChars = itemPrefix.length;
					[textElement.text appendString:itemPrefix];
				} else if ([elementName isEqualToString:@"link"]) {
					BNTextTraitLink* linkTrait = [[BNTextTraitLink alloc] initWithStart:textElement.text.length];
					linkTrait.platform = attributeDict[@"platform"];
					[textElement.traits addObject:linkTrait];
					[self.traitStack addObject:linkTrait];
				} else if ([elementName isEqualToString:@"url"]) {
					BNTextTraitLink* linkTrait = [textElement.traits lastObject];
					linkTrait.platform = attributeDict[@"platform"];
					if ([linkTrait.platform isEqualToString:@"newsapps"]) {
						linkTrait.url = [BNAppDelegate URLforResourceSpec:[NSString stringWithFormat:@"/%@", attributeDict[@"href"]]];
					} else {
						linkTrait.url = [NSURL URLWithString:attributeDict[@"href"]];
					}
				} else if ([elementName isEqualToString:@"firstCreated"] || [elementName isEqualToString:@"lastUpdated"]) {
					self.skippingElements = YES;
				}
			}
		}
	}
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string {
	
	// Trim newlines
	string = [string stringByTrimmingCharactersInSet:[NSCharacterSet newlineCharacterSet]];
	
	if([string length] > 0 && self.currentElement != nil && !self.skippingElements) {
		if ([self.currentElement isKindOfClass:[BNElementText class]]) {
			BNElementText* textElement = (BNElementText*)self.currentElement;
			if ([self.currentElementName isEqualToString:@"altText"]) {
				return;
			}
			if (textElement.text == nil) {
				textElement.text = [NSMutableString stringWithString:string];
			} else {
				[textElement.text appendString:string];
			}
		}
	}
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName {
	
	if (!self.skippingElements) {
		
		if ([elementName isEqualToString:@"question"]) {
			[self popStyle];
		} if ([elementName isEqualToString:@"answer"]) {
			[self popStyle];
		}
		
		
		// Close text traits
		if ([self.currentElement isKindOfClass:[BNElementText class]]) {
			BNElementText* textElement = (BNElementText*)self.currentElement;
			
			if ([elementName isEqualToString:@"listItem"]) {
				[textElement.text appendString:@"\n"];
			}
			
			// Remove last '\n' from last listitem
			if ([elementName isEqualToString:@"list"]) {
				[textElement.text deleteCharactersInRange:NSMakeRange(textElement.text.length-2, 2)];
			}
			
			// Check it's a trait that needs closing (i.e. not an unknown or irrelevant closing tag)
			if (self.traitStack.count >= 1) {
				static NSString* knownTraits = @"bold|italic|link";
				if ([knownTraits rangeOfString:elementName].location != NSNotFound) {
					BNTextTrait* trait = self.traitStack.lastObject;
					[self.traitStack removeLastObject];
					trait.end = textElement.text.length;
				}
			}
		}
	}
	else {
		if ([elementName isEqualToString:@"firstCreated"] || [elementName isEqualToString:@"lastUpdated"]) {
			self.skippingElements = NO;
		}
	}

	
	self.elementDepth--;
	if (self.elementDepth == 0) {
		self.currentElement = nil;
		self.skippingElements = NO;
	}
	
}


@end
*/
