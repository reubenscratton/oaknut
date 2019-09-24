//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "topiclabel.h"
//#import "NSDate+Formatter.h"
//#import "BNStyles.h"
#include "../articles/texttraits.h"
//#import "BNMyNewsCollection.h"
//#import "NSString+URLEncoding.h"
//#import "BNCollections.h"


#define AUTOREFRESH_INTERVAL 30

static AttributedString* s_separatorString;


static AttributedString& getSeparatorString() {
    if (!s_separatorString) {
        s_separatorString = new AttributedString("  |  ");
        s_separatorString->applyStyle(app->getStyle("linkSeparator"));
    }
    return *s_separatorString;
}

BNTopicLabel::BNTopicLabel(BNItem* item, int maxTopics, bool inverseColorScheme, bool hideTimestamp, BNCollection* displayingCollection, bool isLongTimestamp) : _hideTimestamp(hideTimestamp), _longTimestamp(isLongTimestamp), _timestamp(item ? item->_lastUpdated : 0)
{
    setLayoutSize(MEASURESPEC::Wrap(), MEASURESPEC::Wrap());
    applyStyle("topicAndTimestamp");
    
    // Get all collections that this item belongs to, starting with the home collection
    set<BNCollection*> collections;
    if (item->getHomedCollection()) {
        collections.insert(item->getHomedCollection());
    }
    
    for (BNRelationship *parentRelationship : item->_parentRelationships) {
        BNCollection *parentCollection = (BNCollection*)parentRelationship->_parentObject;
        if (parentCollection->canBeTopicLink()) {
            collections.insert(parentCollection);
        }
    }
    
    for (BNRelationship *childRelationship : item->_childRelationships) {
        if (childRelationship->_childObject->isCollection()) {
            BNCollection *relatedCollection = (BNCollection *)childRelationship->_childObject;
            if (relatedCollection->canBeTopicLink()) {
                collections.insert(relatedCollection);
            }
        }
    }
    
    // My News: remove non-followed collections
    if (displayingCollection->_modelId.hasPrefix(BNModelIdMyNews)) {
        for (auto c: collections) {
            if (!BNCollections::isFollowed(c->getStub())) {
                collections.erase(c);
            }
        }
    }
    
    // Remove home collection if this is part of the displayed collection
    if (collections.size() > 1) {
        if (item->getHomedCollection()->_modelId == displayingCollection->_modelId) {
            collections.erase(item->getHomedCollection());
        }
    }
    
    //if (collections.size() > maxTopics) {
    //    [collections removeObjectsInRange:NSMakeRange(maxTopics, collections.count-maxTopics)];
    //}
    
    // Add topic collections
    AttributedString topicLinks;
    for (BNCollection *collection : collections) {
        if (topicLinks.length()) {
            topicLinks.append(getSeparatorString());
        }
        BNTextTrait *linkTrait = new BNTextTrait(BNTextTrait::Type::Link, 0);
        linkTrait->_url = collection->url();
        //NSMutableDictionary *attrs = attrsTopicAndTimestamp.mutableCopy;
        //attrs[NSForegroundColorAttributeName] = inverseColorScheme ? [UIColor whiteColor] : [UIColor bbcNewsLiveRed];
        //attrs[BNLinkAttributeName] = linkTrait;
        //[topicLinks appendAttributedString:[[NSAttributedString alloc] initWithString:collection.name attributes:attrs]];
        topicLinks += collection->_name;
        topicLinks.setAttribute(Attribute::forecolor(0xFF0000cc), topicLinks.length()-collection->_name.length(), topicLinks.length());
    }
    _topicLinks = topicLinks;
    
    _useFullWidth = true;
    updateText();
}

string dateStringFromTimestamp(TIMESTAMP timestamp, bool useLongFormat);

void BNTopicLabel::updateText() {

	// Timestamp
	string timestampText = "";
	if (!_hideTimestamp) {
        timestampText = dateStringFromTimestamp(_timestamp, _longTimestamp);
	}

    AttributedString text = timestampText;
    // TODO text.applyStyle("topicAndTimestamp");
	if (_topicLinks.length()) {
		if (timestampText.length()) {
			text.append(getSeparatorString());
		}
		text.append(_topicLinks);
	}
	
    setText(text);

}
/*
void BNTopicLabelInfo::measureForWidth(float width, POINT offset) {
    BNLabelInfo::measureForWidth(width, offset);
	
	RECT rect = _bounds;
    rect.origin.x -= 5; // todo: fix
	rect.origin.y -= 3;
	_bounds = rect;
}

void BNTopicLabelInfo::createLabel(View* superview) {
    BNLabelInfo::createLabel(superview);
    _timer = Timer::start([=]() {
        updateText();
    }, AUTOREFRESH_INTERVAL, true);
}

void BNTopicLabelInfo::removeLabel() {
    BNLabelInfo::removeLabel();
    if (_timer) {
        _timer->stop();
        _timer = NULL;
    }
}
*/


string dateStringFromTimestamp(TIMESTAMP timestamp, bool useLongFormat) {
    
    // If timestamp null or is in the future, consider it invalid
    TIMESTAMP now = app->currentMillis();
    if (!timestamp || timestamp > now) {
        return "";
    }
    
    time_t t = (time_t)(timestamp);
    time_t t_now = (time_t)(now/1000);
    //double d = difftime(t_now, t);
    auto a = gmtime(&t);
    auto tm = *a;
    a = gmtime(&t_now);
    auto tm_now = *a;


    int dyear = tm_now.tm_year - tm.tm_year;
    int dmonth= tm_now.tm_mon - tm.tm_mon;
    int dday = tm_now.tm_mday - tm.tm_mday;
    int dsec = tm_now.tm_sec - tm.tm_sec;
    int dmin = tm_now.tm_min - tm.tm_min;
    int dhour= tm_now.tm_hour- tm.tm_hour;

    // If article published today
    if (dyear==0 && dmonth==0 && dday==0) {
        if (dsec < 60 && dhour == 0 && dmin == 0) {
            return useLongFormat ? "1 min ago" : "1m";
        } else if (dmin < 2 && dhour == 0) {
            return string::format(useLongFormat ? "%ld min ago" : "%ldm", (long)dmin);
        } else if (dhour < 1) {
            return string::format(useLongFormat ? "%ld mins ago" : "%ldm", (long)dmin);
        } else if (dhour < 2) {
            return string::format(useLongFormat ? "%ld hour ago" : "%ldh", (long)dhour);
        } else {
            return string::format(useLongFormat ? "%ld hours ago": "%ldh", (long)dhour);
        }
    } else {
        
        // If a week or more old, use absolute timestamp
        if (dyear > 0 || dday > 6 || dmonth > 0) {
            char ach[32];
            strftime(ach, 32, dyear?(useLongFormat?"%e %h %Y":"%e %h %y"):"%e %h", &tm);
            return string(ach);
            
        } else {
            if (dday < 2) {
                return string::format(useLongFormat ? "%ld day ago" : "%ldd", (long)dday);
            } else if (dday < 7) {
                return string::format(useLongFormat ? "%ld days ago" : "%ldd", (long)dday);
            }
        }
    }
    
    return "";
}

/*


static NSUInteger const durationCalendarUnits = NSCalendarUnitHour | NSCalendarUnitMinute | NSCalendarUnitSecond;
static NSUInteger const dateCalendarUnits = NSCalendarUnitYear | NSCalendarUnitMonth | NSCalendarUnitDay | NSCalendarUnitHour | NSCalendarUnitMinute | NSCalendarUnitSecond;

static NSCalendar *calendar;
static NSTimeZone* utcTimeZone;



+ (NSDateFormatter*)dateFormatterWithDateFormatLongYear{
    return [self createDateFormatterWithName:@"DateFormatLongYear" andFormat:@"yyyy"];
}

@end

@implementation NSDate (Formatter)

+ (NSString*)durationStringFromTimeIntervalNumber:(NSInteger)timeInterval {
    
    NSDate *dateNow = [self dateNowInUTC];
    
    // Trevor provides this interval in milliseconds. We need to process it in seconds.
    NSTimeInterval interval = timeInterval / 1000.0;
    NSDate *newDate = [dateNow dateByAddingTimeInterval:interval];
    
    // order of dateNow to newDate to establish time in the future.
    NSDateComponents *dateComponent = [calendar components:durationCalendarUnits fromDate:dateNow toDate:newDate options:NSCalendarWrapComponents];
    
    if (!dateComponent) {
        return @"";
    }
    
    if (dateComponent.hour > 0) {
        return [NSString stringWithFormat:@"%ld:%02ld:%02ld", (long)dateComponent.hour, (long)dateComponent.minute, (long)dateComponent.second];
        
    } else if (dateComponent.minute > 0) {
        
        return [NSString stringWithFormat:@"%02ld:%02ld", (long)dateComponent.minute, (long)dateComponent.second];
        
    } else {//if (dateComponent.second > 0) {
        
        return [NSString stringWithFormat:@"00:%02ld", (long)dateComponent.second];
        
    }
}

+ (NSString*)accessibleDurationStringFromTimeIntervalNumber:(NSInteger)timeInterval {
    NSDate *dateNow = [self dateNowInUTC];
    
    // Trevor provides this interval in milliseconds. We need to process it in seconds.
    NSTimeInterval interval = timeInterval / 1000.0;
    NSDate *newDate = [dateNow dateByAddingTimeInterval:interval];
    
    // order of dateNow to newDate to establish time in the future.
    NSDateComponents *dateComponent = [calendar components:durationCalendarUnits fromDate:dateNow toDate:newDate options:NSCalendarWrapComponents];
    
    if (!dateComponent) {
        return @"";
    }
    
    if (dateComponent.hour > 0) {
        if (dateComponent.hour == 1) {
            return [NSString stringWithFormat:@"%ld hour, %ld minutes and %ld seconds", (long)dateComponent.hour, (long)dateComponent.minute, (long)dateComponent.second];
        }
        return [NSString stringWithFormat:@"%ld hours, %ld minutes and %ld seconds", (long)dateComponent.hour, (long)dateComponent.minute, (long)dateComponent.second];
        
    } else if (dateComponent.minute > 0) {
        if (dateComponent.minute == 1) {
            return [NSString stringWithFormat:@"%ld minute and %ld seconds", (long)dateComponent.minute, (long)dateComponent.second];
        }
        return [NSString stringWithFormat:@"%ld minutes and %ld seconds", (long)dateComponent.minute, (long)dateComponent.second];
        
    } else {//if (dateComponent.second > 0) {
        if (dateComponent.second == 0) {
            return [NSString stringWithFormat:@"%ld second", (long)dateComponent.second];
        }
        return [NSString stringWithFormat:@"%ld seconds", (long)dateComponent.second];
    }
}

+ (BOOL)publishedToday:(NSDateComponents*)dateComponents {
    return dateComponents.year == 0 &&
    dateComponents.month == 0 &&
    dateComponents.day == 0;
}

+ (NSString *)longYearStringFromTimeStamp:(NSInteger)timeInterval {
    
    NSDate *processedDate = [NSDate dateWithTimeIntervalSince1970:timeInterval];
    
    NSDateFormatter *yearFormatter = [self dateFormatterWithDateFormatLongYear];
    
    if(!yearFormatter){
        return @"";
    }
    
    NSString *yearDate = [yearFormatter stringFromDate:processedDate];
    return yearDate;
}

+ (NSString *)longYearStringFromDate:(NSDate*)date {
    NSDateFormatter *yearFormatter = [self dateFormatterWithDateFormatLongYear];
    return [yearFormatter stringFromDate:date];
}

+ (NSString *)tinyTimestampStringFromTimeStamp:(NSInteger)timeInterval {
    NSDate *timestamp = [NSDate dateWithTimeIntervalSince1970:timeInterval];
    NSDate *dateNow = [self dateNowInUTC];
    
    if ([dateNow compare:timestamp] == NSOrderedAscending) {
        return nil;
    }
    NSDateComponents *components = [calendar components:dateCalendarUnits fromDate:timestamp toDate:dateNow options:NSCalendarWrapComponents];
    if (!components) {
        return nil;
    }
    if (components.year>0 || components.month>0) {
        return nil;
    }
    if (components.day>0) return [NSString stringWithFormat:@"%ldd", (long)components.day];
    if (components.hour>0) return [NSString stringWithFormat:@"%ldh", (long)components.hour];
    return [NSString stringWithFormat:@"%ldm", (long)components.minute];
}

+ (NSDateFormatter*)analyticsDateLogDateFormatter {
    
    return [self createDateFormatterWithName:@"BNLastLogDateDateFormatter" andFormat:@"yyyy-MM-dd"];
}

+ (NSDateFormatter*)analyticsDateTimeLogDateFormatter {
    
    return [self createDateFormatterWithName:@"BNLastLogDateTimeDateFormatter" andFormat:@"HH"];
}

// Is this right?
+ (NSDate*)dateNowInUTC {
    // we want to compare based on UTC now not locale now. This is to prevent going bakcwards/forwards in time.
    NSInteger seconds = -[utcTimeZone secondsFromGMTForDate:[NSDate date]];
    return [NSDate dateWithTimeInterval:seconds sinceDate:[NSDate date]];
}

@end
*/

