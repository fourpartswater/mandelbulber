/*
 * timeline.cpp
 *
 *  Created on: 2010-09-20
 *      Author: krzysztof
 */

#include <gtk-2.0/gtk/gtk.h>
#include "timeline.hpp"
#include "Render3D.h"
#include "interface.h"
#include "settings.h"
#include "cimage.hpp"
#include "callbacks.h"

cTimeline *timeline;

cTimeline::cTimeline()
{
	database = new cDatabase(1);
	keyframeCount = 0;
	isCreated = false;
}

cTimeline::~cTimeline()
{
	delete database;
}

int cTimeline::Initialize(char *keyframesPath)
{
	int numberOfKeyframes = CheckNumberOfKeyframes(keyframesPath);
	printf("Found %d keyframes\n", numberOfKeyframes);

	cImage *thumbnail;

	CreateInterface(numberOfKeyframes);

	char filename2[1000];
	sTimelineRecord *record = new sTimelineRecord;
	for (int i = 0; i < numberOfKeyframes; i++)
	{
		thumbnail = new cImage(128, 128);
		IndexFilename(filename2, keyframesPath, (char*) "fract", i);
		ThumbnailRender(filename2, thumbnail);
		thumbnail->CreatePreview(1.0);
		thumbnail->ConvertTo8bit();
		thumbnail->UpdatePreview();
		memcpy(record->thumbnail, thumbnail->GetPreviewPtr(), sizeof(sRGB8) * 128 * 128);
		record->index = i; //only for testing database

		if (i == 0)	database->SetRecord(0, (char*) record, sizeof(sTimelineRecord));
		else database->AddRecord((char*) record, sizeof(sTimelineRecord));

		DisplayInDrawingArea(i, timelineInterface.darea[i]);
		while (gtk_events_pending())
			gtk_main_iteration();
		gtk_signal_connect(GTK_OBJECT(timelineInterface.darea[i]), "expose-event", GTK_SIGNAL_FUNC(thumbnail_expose), NULL);
		delete thumbnail;
	}

	delete record;

	isCreated = true;
	keyframeCount = numberOfKeyframes;
	return numberOfKeyframes;
}

int cTimeline::CheckNumberOfKeyframes(char *keyframesPath)
{
	char filename2[1000];
	int maxKeyNumber = 0;
	do
	{
		IndexFilename(filename2, keyframesPath, (char*) "fract", maxKeyNumber);
		maxKeyNumber++;
	} while (FileIfExist(filename2));
	maxKeyNumber--;
	return maxKeyNumber;
}

void cTimeline::GetImage(int index, sRGB8 *image)
{
	sTimelineRecord *record = new sTimelineRecord;
	database->GetRecord(index, (char*) record);
	memcpy(image, record->thumbnail, sizeof(sRGB8) * 128 * 128);
	delete record;
}

void cTimeline::DisplayInDrawingArea(int index, GtkWidget *darea)
{
	sRGB8 *image = new sRGB8[128 * 128];
	timeline->GetImage(index, image);
	gdk_draw_rgb_image(darea->window, darea->style->fg_gc[GTK_STATE_NORMAL], 0, 0, 128, 128, GDK_RGB_DITHER_MAX, (unsigned char*) image, 128 * 3);
	delete[] image;
}

void cTimeline::CreateInterface(int numberOfKeyframes)
{

	timelineInterface.darea = new GtkWidget*[numberOfKeyframes];
	timelineInterface.table = gtk_table_new(2, numberOfKeyframes + 1, false);
	gtk_container_add(GTK_CONTAINER(timeLineWindow), timelineInterface.table);
	gtk_widget_show(timelineInterface.table);

	char widgetName[7];

	for (int i = 0; i < numberOfKeyframes; i++)
	{
		timelineInterface.darea[i] = gtk_drawing_area_new();
		sprintf(widgetName, "da%d", i);
		gtk_widget_set_size_request(timelineInterface.darea[i], 128, 128);
		gtk_widget_set_name(timelineInterface.darea[i], widgetName);

		gtk_table_attach_defaults(GTK_TABLE(timelineInterface.table), timelineInterface.darea[i], i, i + 1, 1, 2);
		gtk_widget_show(timelineInterface.darea[i]);
	}
}

void cTimeline::RebulidTimelineWindow(void)
{
	CreateInterface(keyframeCount);
	for(int i=0; i<keyframeCount; i++)
	{
		DisplayInDrawingArea(i, timelineInterface.darea[i]);
		gtk_signal_connect(GTK_OBJECT(timelineInterface.darea[i]), "expose-event", GTK_SIGNAL_FUNC(thumbnail_expose), NULL);
	}
}

gboolean thumbnail_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
	const char* widgetName = gtk_widget_get_name(widget);
	int number = 0;
	sscanf(widgetName, "da%d", &number);
	timeline->DisplayInDrawingArea(number, widget);
	while (gtk_events_pending())
		gtk_main_iteration();
	return true;
}

