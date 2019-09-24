//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "constants.h"

const char* BNModelTypeCollection   = "bbc.mobile.news.collection";
const char* BNModelTypeItem          = "bbc.mobile.news.item";
const char* BNModelTypeLiveEvent      = "bbc.mobile.news.live_event";
const char* BNModelTypeLiveEventExt = "bbc.mobile.news.live_event_ext";
const char* BNModelTypeCommentary      = "bbc.mobile.news.commentary";
const char* BNModelTypePerson          = "bbc.mobile.news.person";
const char* BNModelTypeImage          = "bbc.mobile.news.image";
const char* BNModelTypeAudio          = "bbc.mobile.news.audio";
const char* BNModelTypeVideo          = "bbc.mobile.news.video";
const char* BNModelTypeUser          = "bbc.mobile.news.user";
const char* BNModelTypeByline       = "bbc.mobile.news.byline";
const char* BNModelTypeUnknown      = "bbc.mobile.news.unknown"; // Sometimes we don't know the type upfront

// this appears to be some new kind of Item in that's used as "type" value in collections of items, despite
// clearly belonging in a different namespace. Good old auntie...
const char* BNModelTypeLink          = "bbc.mobile.news.group.link";


const char* BNModelIdPolicyPrefix   = "/policy";
const char* BNModelIdUser           = "/user";
const char* BNModelIdTopStories     = "/cps/news/front_page";
const char* BNModelIdSportTopStories= "/cps/sport/front-page";
const char* BNModelIdMagazine       = "/cps/news/magazine";
const char* BNModelIdLive           = "/cps/news/10318089"; //@"/cps/news/10473693";
const char* BNModelIdMostPopular    = "/most_popular/news";
const char* BNModelIdMostRead       = "/most_popular/news/read";
const char* BNModelIdMostWatched    = "/most_popular/news/watched";
const char* BNModelIdMyNews         = "/newsapps/mynews";
const char* BNModelIdTrending       = "/trending/events";
const char* BNModelIdHandrail       = "/handrail";
const char* BNModelIdSettings       = "@settings";
const char* BNModelIdHelp           = "/cps/news/help-30119604";
const char* BNModelIdTestArticle    = "/cps/news/22763404";
const char* BNModelIdFollowScreen   = "/followscreen";



const char* BNContentFormatTextual       = "bbc.mobile.news.format.textual";
const char* BNContentFormatVideo         = "bbc.mobile.news.format.video";
const char* BNContentFormatAudio         = "bbc.mobile.news.format.audio";
const char* BNContentFormatPhotoGallery  = "bbc.mobile.news.format.photogallery";
const char* BNContentFormatLiveevent     = "bbc.mobile.news.format.liveevent";
const char* BNContentFormatNewsapps      = "bbc.mobile.news.format.newsapps";
const char* BNContentFormatInternal      = "bbc.mobile.news.format.internal";
const char* BNContentFormatInteractive   = "bbc.mobile.news.format.interactive";
const char* BNContentFormatCommentary    = "bbc.mobile.news.format.commentary";
const char* BNContentFormatCorrespondent = "bbc.mobile.news.format.correspondent";
const char* BNContentFormatMedia         = "bbc.mobile.news.format.media";
const char* BNContentFormatIndex         = "bbc.mobile.news.legacy.idx";
const char* BNContentFormatFixture       = "bbc.mobile.news.legacy.fix";
const char* BNContentFormatHyperpuff     = "bbc.mobile.news.legacy.hyp";
const char* BNContentFormatEvent         = "bbc.linkeddata.concepts.event";
const char* BNContentFormatOrganisation  = "bbc.linkeddata.concepts.organisation";
const char* BNContentFormatPerson        = "bbc.linkeddata.concepts.person";
const char* BNContentFormatPlace         = "bbc.linkeddata.concepts.place";
const char* BNContentFormatTheme         = "bbc.linkeddata.concepts.theme";
const char* BNContentFormatStoryline     = "bbc.linkeddata.concepts.storyline";

const char* BNContentSiteNews  = "/news";
const char* BNContentSiteSport  = "/sport";

const char* BNRelationshipTypeDefaultContent          = "bbc.mobile.news.content.default";
const char* BNRelationshipTypeStartingContent         = "bbc.mobile.news.content.starting";
const char* BNRelationshipTypeHomeCollection          = "bbc.mobile.news.home_section";
const char* BNRelationshipTypePlacementIndex          = "bbc.mobile.news.placement.index";
const char* BNRelationshipTypePlacementBody           = "bbc.mobile.news.placement.body";
const char* BNRelationshipTypePlacementPrimary        = "bbc.mobile.news.placement.primary";
const char* BNRelationshipTypePlacementPhotoGallery   = "bbc.mobile.news.placement.photogallery";
const char* BNRelationshipTypePlacementIndexSlideshow = "bbc.mobile.news.placement.slideshow";
const char* BNRelationshipTypeGroupFeatureAnalysis    = "bbc.mobile.news.group.feature-main.analysis";
const char* BNRelationshipTypeGroupFeatureMainPrefix  = "bbc.mobile.news.group.feature-main";
const char* BNRelationshipTypeGroupAlsoInTheNews      = "bbc.mobile.news.group.also-in-news.also-in-the-news";
const char* BNRelationshipTypePlacementPosterImage    = "bbc.mobile.news.placement.poster";
const char* BNRelationshipTypeTopic                   = "bbc.mobile.news.topic";
const char* BNRelationshipTypeHandrailCollection      = "bbc.mobile.news.handrail";
const char* BNRelationshipTypeFollowingExplicit       = "bbc.mobile.news.following.explicit";
const char* BNRelationshipTypeCommentaryComment       = "bbc.mobile.news.commentary.type.comment";
const char* BNRelationshipTypeCommentaryTweet         = "bbc.mobile.news.commentary.type.tweet";
const char* BNRelationshipTypeCommentaryEmail         = "bbc.mobile.news.commentary.type.email";
const char* BNRelationshipTypeCommentarySMS           = "bbc.mobile.news.commentary.type.sms";
const char* BNRelationshipTypeCommentaryBlog          = "bbc.mobile.news.commentary.type.blog";

const char* BNCommentaryTypeComment               = "COMMENT";
const char* BNCommentaryTypeTweet                 = "TWEET";
const char* BNCommentaryTypeEmail                 = "EMAIL";
const char* BNCommentaryTypeSMS                   = "SMS";
const char* BNCommentaryTypeBlog                  = "BLOG";

