//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "constants.h"

const string BNModelTypeCollection   = "bbc.mobile.news.collection";
const string BNModelTypeItem          = "bbc.mobile.news.item";
const string BNModelTypeLiveEvent      = "bbc.mobile.news.live_event";
const string BNModelTypeLiveEventExt = "bbc.mobile.news.live_event_ext";
const string BNModelTypeCommentary      = "bbc.mobile.news.commentary";
const string BNModelTypePerson          = "bbc.mobile.news.person";
const string BNModelTypeImage          = "bbc.mobile.news.image";
const string BNModelTypeAudio          = "bbc.mobile.news.audio";
const string BNModelTypeVideo          = "bbc.mobile.news.video";
const string BNModelTypeUser          = "bbc.mobile.news.user";
const string BNModelTypeByline       = "bbc.mobile.news.byline";
const string BNModelTypeUnknown      = "bbc.mobile.news.unknown"; // Sometimes we don't know the type upfront

// this appears to be some new kind of Item in that's used as "type" value in collections of items, despite
// clearly belonging in a different namespace. Good old auntie...
const string BNModelTypeLink          = "bbc.mobile.news.group.link";


const string BNModelIdPolicyPrefix   = "/policy";
const string BNModelIdUser           = "/user";
const string BNModelIdTopStories     = "/cps/news/front_page";
const string BNModelIdSportTopStories= "/cps/sport/front-page";
const string BNModelIdMagazine       = "/cps/news/magazine";
const string BNModelIdLive           = "/cps/news/10318089"; //@"/cps/news/10473693";
const string BNModelIdMostPopular    = "/most_popular/news";
const string BNModelIdMostRead       = "/most_popular/news/read";
const string BNModelIdMostWatched    = "/most_popular/news/watched";
const string BNModelIdMyNews         = "/newsapps/mynews";
const string BNModelIdTrending       = "/trending/events";
const string BNModelIdHandrail       = "/handrail";
const string BNModelIdSettings       = "@settings";
const string BNModelIdHelp           = "/cps/news/help-30119604";
const string BNModelIdTestArticle    = "/cps/news/22763404";
const string BNModelIdFollowScreen   = "/followscreen";



const string BNContentFormatTextual       = "bbc.mobile.news.format.textual";
const string BNContentFormatVideo         = "bbc.mobile.news.format.video";
const string BNContentFormatAudio         = "bbc.mobile.news.format.audio";
const string BNContentFormatPhotoGallery  = "bbc.mobile.news.format.photogallery";
const string BNContentFormatLiveevent     = "bbc.mobile.news.format.liveevent";
const string BNContentFormatNewsapps      = "bbc.mobile.news.format.newsapps";
const string BNContentFormatInternal      = "bbc.mobile.news.format.internal";
const string BNContentFormatInteractive   = "bbc.mobile.news.format.interactive";
const string BNContentFormatCommentary    = "bbc.mobile.news.format.commentary";
const string BNContentFormatCorrespondent = "bbc.mobile.news.format.correspondent";
const string BNContentFormatMedia         = "bbc.mobile.news.format.media";
const string BNContentFormatIndex         = "bbc.mobile.news.legacy.idx";
const string BNContentFormatFixture       = "bbc.mobile.news.legacy.fix";
const string BNContentFormatHyperpuff     = "bbc.mobile.news.legacy.hyp";
const string BNContentFormatEvent         = "bbc.linkeddata.concepts.event";
const string BNContentFormatOrganisation  = "bbc.linkeddata.concepts.organisation";
const string BNContentFormatPerson        = "bbc.linkeddata.concepts.person";
const string BNContentFormatPlace         = "bbc.linkeddata.concepts.place";
const string BNContentFormatTheme         = "bbc.linkeddata.concepts.theme";
const string BNContentFormatStoryline     = "bbc.linkeddata.concepts.storyline";

const string BNContentSiteNews  = "/news";
const string BNContentSiteSport  = "/sport";

const string BNRelationshipTypeDefaultContent          = "bbc.mobile.news.content.default";
const string BNRelationshipTypeStartingContent         = "bbc.mobile.news.content.starting";
const string BNRelationshipTypeHomeCollection          = "bbc.mobile.news.home_section";
const string BNRelationshipTypePlacementIndex          = "bbc.mobile.news.placement.index";
const string BNRelationshipTypePlacementBody           = "bbc.mobile.news.placement.body";
const string BNRelationshipTypePlacementPrimary        = "bbc.mobile.news.placement.primary";
const string BNRelationshipTypePlacementPhotoGallery   = "bbc.mobile.news.placement.photogallery";
const string BNRelationshipTypePlacementIndexSlideshow = "bbc.mobile.news.placement.slideshow";
const string BNRelationshipTypeGroupFeatureAnalysis    = "bbc.mobile.news.group.feature-main.analysis";
const string BNRelationshipTypeGroupFeatureMainPrefix  = "bbc.mobile.news.group.feature-main";
const string BNRelationshipTypeGroupAlsoInTheNews      = "bbc.mobile.news.group.also-in-news.also-in-the-news";
const string BNRelationshipTypePlacementPosterImage    = "bbc.mobile.news.placement.poster";
const string BNRelationshipTypeTopic                   = "bbc.mobile.news.topic";
const string BNRelationshipTypeHandrailCollection      = "bbc.mobile.news.handrail";
const string BNRelationshipTypeFollowingExplicit       = "bbc.mobile.news.following.explicit";
const string BNRelationshipTypeCommentaryComment       = "bbc.mobile.news.commentary.type.comment";
const string BNRelationshipTypeCommentaryTweet         = "bbc.mobile.news.commentary.type.tweet";
const string BNRelationshipTypeCommentaryEmail         = "bbc.mobile.news.commentary.type.email";
const string BNRelationshipTypeCommentarySMS           = "bbc.mobile.news.commentary.type.sms";
const string BNRelationshipTypeCommentaryBlog          = "bbc.mobile.news.commentary.type.blog";

const string BNCommentaryTypeComment               = "COMMENT";
const string BNCommentaryTypeTweet                 = "TWEET";
const string BNCommentaryTypeEmail                 = "EMAIL";
const string BNCommentaryTypeSMS                   = "SMS";
const string BNCommentaryTypeBlog                  = "BLOG";

