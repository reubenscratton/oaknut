//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "constants.h"

const string BNModelTypeCollection   = ".collection";
const string BNModelTypeItem          = ".item";
const string BNModelTypeLiveEvent      = ".live_event";
const string BNModelTypeLiveEventExt = ".live_event_ext";
const string BNModelTypeCommentary      = ".commentary";
const string BNModelTypePerson          = ".person";
const string BNModelTypeImage          = ".image";
const string BNModelTypeAudio          = ".audio";
const string BNModelTypeVideo          = ".video";
const string BNModelTypeUser          = ".user";
const string BNModelTypeByline       = ".byline";
const string BNModelTypeUnknown      = ".unknown"; // Sometimes we don't know the type upfront

// this appears to be some new kind of Item in that's used as "type" value in collections of items, despite
// clearly belonging in a different namespace. Good old auntie...
const string BNModelTypeLink          = ".group.link";


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



const string BNContentFormatTextual       = ".format.textual";
const string BNContentFormatVideo         = ".format.video";
const string BNContentFormatAudio         = ".format.audio";
const string BNContentFormatPhotoGallery  = ".format.photogallery";
const string BNContentFormatLiveevent     = ".format.liveevent";
const string BNContentFormatNewsapps      = ".format.newsapps";
const string BNContentFormatInternal      = ".format.internal";
const string BNContentFormatInteractive   = ".format.interactive";
const string BNContentFormatCommentary    = ".format.commentary";
const string BNContentFormatCorrespondent = ".format.correspondent";
const string BNContentFormatMedia         = ".format.media";
const string BNContentFormatIndex         = ".legacy.idx";
const string BNContentFormatFixture       = ".legacy.fix";
const string BNContentFormatHyperpuff     = ".legacy.hyp";
const string BNContentFormatEvent         = "bbc.linkeddata.concepts.event";
const string BNContentFormatOrganisation  = "bbc.linkeddata.concepts.organisation";
const string BNContentFormatPerson        = "bbc.linkeddata.concepts.person";
const string BNContentFormatPlace         = "bbc.linkeddata.concepts.place";
const string BNContentFormatTheme         = "bbc.linkeddata.concepts.theme";
const string BNContentFormatStoryline     = "bbc.linkeddata.concepts.storyline";

const string BNContentSiteNews  = "/news";
const string BNContentSiteSport  = "/sport";

const string BNRelationshipTypeDefaultContent          = ".content.default";
const string BNRelationshipTypeStartingContent         = ".content.starting";
const string BNRelationshipTypeHomeCollection          = ".home_section";
const string BNRelationshipTypePlacementIndex          = ".placement.index";
const string BNRelationshipTypePlacementBody           = ".placement.body";
const string BNRelationshipTypePlacementPrimary        = ".placement.primary";
const string BNRelationshipTypePlacementPhotoGallery   = ".placement.photogallery";
const string BNRelationshipTypePlacementIndexSlideshow = ".placement.slideshow";
const string BNRelationshipTypeGroupFeatureAnalysis    = ".group.feature-main.analysis";
const string BNRelationshipTypeGroupFeatureMainPrefix  = ".group.feature-main";
const string BNRelationshipTypeGroupAlsoInTheNews      = ".group.also-in-news.also-in-the-news";
const string BNRelationshipTypePlacementPosterImage    = ".placement.poster";
const string BNRelationshipTypeTopic                   = ".topic";
const string BNRelationshipTypeHandrailCollection      = ".handrail";
const string BNRelationshipTypeFollowingExplicit       = ".following.explicit";
const string BNRelationshipTypeCommentaryComment       = ".commentary.type.comment";
const string BNRelationshipTypeCommentaryTweet         = ".commentary.type.tweet";
const string BNRelationshipTypeCommentaryEmail         = ".commentary.type.email";
const string BNRelationshipTypeCommentarySMS           = ".commentary.type.sms";
const string BNRelationshipTypeCommentaryBlog          = ".commentary.type.blog";

const string BNCommentaryTypeComment               = "COMMENT";
const string BNCommentaryTypeTweet                 = "TWEET";
const string BNCommentaryTypeEmail                 = "EMAIL";
const string BNCommentaryTypeSMS                   = "SMS";
const string BNCommentaryTypeBlog                  = "BLOG";

