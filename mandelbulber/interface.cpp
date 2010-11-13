/********************************************************
 /                   MANDELBULBER                        *
 /                                                       *
 / author: Krzysztof Marczak                             *
 / contact: buddhi1980@gmail.com                         *
 / licence: GNU GPL                                      *
 ********************************************************/

/*
 * interface.cpp
 *
 *  Created on: 2010-01-23
 *      Author: krzysztof
 */

#include <gtk-2.0/gtk/gtk.h>
#include "interface.h"
#include "image.h"
#include "callbacks.h"
#include <stdlib.h>
#include <math.h>
#include "settings.h"
#include <string.h>

sInterface Interface;
sInterface_data Interface_data;
sNoGUIdata noGUIdata;
GtkWidget *window2, *window_histogram, *window_interface;
GtkWidget *darea, *darea2, *darea3;
GtkWidget *dareaPalette;
int scrollbarSize;
int lastWindowWidth;
int lastWindowHeight;

sTimelineInterface timelineInterface;
GtkWidget *timeLineWindow = 0;

char lastFilenameImage[1000];
char lastFilenameSettings[1000];
char lastFilenamePalette[1000];

int x_mouse = 0;
int y_mouse = 0;
bool programClosed = false;
bool interfaceCreated = false;
bool paletteViewCreated = false;

GtkWidget* CreateEdit(const gchar *text, const gchar *txt_label, int size, GtkWidget *edit)
{
	GtkWidget *box = gtk_hbox_new(FALSE, 1);
	gtk_entry_set_text(GTK_ENTRY(edit), text);
	gtk_entry_set_width_chars(GTK_ENTRY(edit), size);
	GtkWidget *label = gtk_label_new(txt_label);
	gtk_box_pack_start(GTK_BOX(box), label, false, false, 1);
	gtk_box_pack_start(GTK_BOX(box), edit, false, false, 1);
	return box;
}

GtkWidget* CreateWidgetWithLabel(const gchar *txt_label, GtkWidget *widget)
{
	GtkWidget *box = gtk_hbox_new(FALSE, 1);
	GtkWidget *label = gtk_label_new(txt_label);
	gtk_box_pack_start(GTK_BOX(box), label, false, false, 1);
	gtk_box_pack_start(GTK_BOX(box), widget, false, false, 1);
	return box;
}

void CreateFilesDialog(GtkWidget *widget, gpointer data)
{
	sDialogFiles *dialog = new sDialogFiles;

	dialog->window_files = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog->window_files), "File paths");
	gtk_container_set_border_width(GTK_CONTAINER(dialog->window_files), 10);

	dialog->box_main = gtk_vbox_new(FALSE, 1);
	dialog->box_buttons = gtk_hbox_new(FALSE, 1);

	dialog->table_edits = gtk_table_new(7, 4, false);

	dialog->label_destination = gtk_label_new("Destination image sequnce");
	dialog->label_background = gtk_label_new("Background");
	dialog->label_envmap = gtk_label_new("Environment map (512x512)");
	dialog->label_lightmap = gtk_label_new("Ambient occlusion color map (512x512)");
	dialog->label_path = gtk_label_new("Animation path");
	dialog->label_keyframes = gtk_label_new("Keyframe sequence");
	dialog->label_fileSound = gtk_label_new("Sound file (*.wav)");

	dialog->edit_destination = gtk_entry_new();
	dialog->edit_background = gtk_entry_new();
	dialog->edit_envmap = gtk_entry_new();
	dialog->edit_lightmap = gtk_entry_new();
	dialog->edit_path = gtk_entry_new();
	dialog->edit_keyframes = gtk_entry_new();
	dialog->edit_sound = gtk_entry_new();

	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_destination), 100);
	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_background), 100);
	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_envmap), 100);
	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_lightmap), 100);
	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_path), 100);
	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_keyframes), 100);
	gtk_entry_set_width_chars(GTK_ENTRY(dialog->edit_sound), 100);

	gtk_entry_set_text(GTK_ENTRY(dialog->edit_destination), Interface_data.file_destination);
	gtk_entry_set_text(GTK_ENTRY(dialog->edit_background), Interface_data.file_background);
	gtk_entry_set_text(GTK_ENTRY(dialog->edit_envmap), Interface_data.file_envmap);
	gtk_entry_set_text(GTK_ENTRY(dialog->edit_lightmap), Interface_data.file_lightmap);
	gtk_entry_set_text(GTK_ENTRY(dialog->edit_path), Interface_data.file_path);
	gtk_entry_set_text(GTK_ENTRY(dialog->edit_keyframes), Interface_data.file_keyframes);
	gtk_entry_set_text(GTK_ENTRY(dialog->edit_sound), Interface_data.file_sound);

	dialog->bu_cancel = gtk_button_new_with_label("Cancel");
	dialog->bu_ok = gtk_button_new_with_label("OK");
	dialog->bu_select_destination = gtk_button_new_with_label("Select");
	dialog->bu_select_background = gtk_button_new_with_label("Select");
	dialog->bu_select_envmap = gtk_button_new_with_label("Select");
	dialog->bu_select_lightmap = gtk_button_new_with_label("Select");
	dialog->bu_select_path = gtk_button_new_with_label("Select");
	dialog->bu_select_keyframes = gtk_button_new_with_label("Select");
	dialog->bu_select_sound = gtk_button_new_with_label("Select");

	g_signal_connect(G_OBJECT(dialog->bu_ok), "clicked", G_CALLBACK(PressedOkDialogFiles), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_cancel), "clicked", G_CALLBACK(PressedCancelDialogFiles), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_destination), "clicked", G_CALLBACK(PressedSelectDestination), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_background), "clicked", G_CALLBACK(PressedSelectBackground), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_envmap), "clicked", G_CALLBACK(PressedSelectEnvmap), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_lightmap), "clicked", G_CALLBACK(PressedSelectLightmap), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_path), "clicked", G_CALLBACK(PressedSelectFlightPath), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_keyframes), "clicked", G_CALLBACK(PressedSelectKeyframes), dialog);
	g_signal_connect(G_OBJECT(dialog->bu_select_sound), "clicked", G_CALLBACK(PressedSelectSound), dialog);

	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_destination, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_background, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_envmap, 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_lightmap, 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_path, 0, 1, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_keyframes, 0, 1, 5, 6);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->label_fileSound, 0, 1, 6, 7);

	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_destination, 1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_background, 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_envmap, 1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_lightmap, 1, 2, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_path, 1, 2, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_keyframes, 1, 2, 5, 6);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->edit_sound, 1, 2, 6, 7);

	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_destination, 2, 3, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_background, 2, 3, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_envmap, 2, 3, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_lightmap, 2, 3, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_path, 2, 3, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_keyframes, 2, 3, 5, 6);
	gtk_table_attach_defaults(GTK_TABLE(dialog->table_edits), dialog->bu_select_sound, 2, 3, 6, 7);

	gtk_box_pack_start(GTK_BOX(dialog->box_main), dialog->table_edits, false, false, 1);

	gtk_box_pack_start(GTK_BOX(dialog->box_main), dialog->box_buttons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(dialog->box_buttons), dialog->bu_cancel, true, true, 1);
	gtk_box_pack_start(GTK_BOX(dialog->box_buttons), dialog->bu_ok, true, true, 1);

	gtk_container_add(GTK_CONTAINER(dialog->window_files), dialog->box_main);

	gtk_widget_show_all(dialog->window_files);
}

double atofData(const gchar *text, sAddData *addData)
{
	double retval = atof(text);

	if (addData != 0)
	{
		char text2[1000];
		strcpy(text2, text);

		char testmode[1000];
		strcpy(testmode, "null");
		double testamp;
		sscanf(text2, "%s %lg", testmode, &testamp);
		if (!strcmp(testmode, "null"))
		{
			addData->mode = soundNone;
			addData->amp = 0;
			addData->active = true;
		}
		else if (!strcmp(testmode, "s0"))
		{
			addData->mode = soundEnvelope;
			addData->amp = testamp;
			addData->active = true;
		}
		else if (!strcmp(testmode, "sa"))
		{
			addData->mode = soundA;
			addData->amp = testamp;
			addData->active = true;
		}
		else if (!strcmp(testmode, "sb"))
		{
			addData->mode = soundB;
			addData->amp = testamp;
			addData->active = true;
		}
		else if (!strcmp(testmode, "sc"))
		{
			addData->mode = soundC;
			addData->amp = testamp;
			addData->active = true;
		}
		else if (!strcmp(testmode, "sd"))
		{
			addData->mode = soundD;
			addData->amp = testamp;
			addData->active = true;
		}
		else
		{
			addData->mode = soundNone;
			addData->amp = 0;
			addData->active = false;
			retval = atof(text);
		}
	}
	return retval;
}

enumFractalFormula FormulaNumberGUI2Data(int formula)
{
	enumFractalFormula formula2 = trig;
	if (formula == 0) formula2 = none;
	if (formula == 1) formula2 = trig;
	if (formula == 2) formula2 = fast_trig;
	if (formula == 3) formula2 = minus_fast_trig;
	if (formula == 4) formula2 = xenodreambuie;
	if (formula == 5) formula2 = hypercomplex;
	if (formula == 6) formula2 = quaternion;
	if (formula == 7) formula2 = menger_sponge;
	if (formula == 8) formula2 = tglad;
	if (formula == 9) formula2 = kaleidoscopic;
	if (formula == 10) formula2 = mandelbulb2;
	if (formula == 11) formula2 = mandelbulb3;
	if (formula == 12) formula2 = mandelbulb4;
	return formula2;
}

int FormulaNumberData2GUI(enumFractalFormula formula)
{
	int formula2 = 0;
	if (formula == none) formula2 = 0;
	if (formula == trig) formula2 = 1;
	if (formula == fast_trig) formula2 = 2;
	if (formula == minus_fast_trig) formula2 = 3;
	if (formula == xenodreambuie) formula2 = 4;
	if (formula == hypercomplex) formula2 = 5;
	if (formula == quaternion) formula2 = 6;
	if (formula == menger_sponge) formula2 = 7;
	if (formula == tglad) formula2 = 8;
	if (formula == kaleidoscopic) formula2 = 9;
	if (formula == mandelbulb2) formula2 = 10;
	if (formula == mandelbulb3) formula2 = 11;
	if (formula == mandelbulb4) formula2 = 12;
	return formula2;
}

//read data from interface
void ReadInterface(sParamRender *params, sParamSpecial *special)
{
	bool freeSpecial = false;
	if (special == 0)
	{
		special = new sParamSpecial;
	        freeSpecial = true;
	}

	if (!noGUI)
	{
		params->doubles.vp.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_va)), &special->vpX);
		params->doubles.vp.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_vb)), &special->vpY);
		params->doubles.vp.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_vc)), &special->vpZ);
		params->doubles.alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_alfa)), &special->alfa) / 180.0 * M_PI;
		params->doubles.beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_beta)), &special->beta) / 180.0 * M_PI;
		params->doubles.gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_gammaAngle)), &special->gamma) / 180.0 * M_PI;
		params->doubles.zoom = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_zoom)), &special->zoom);
		params->doubles.persp = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_persp)), &special->persp);
		params->doubles.DE_factor = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_DE_stepFactor)), &special->DE_factor);
		params->doubles.quality = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_DE_thresh)), &special->quality);
		params->N = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_maxN)));
		params->minN = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_minN)));
		params->doubles.power = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_power)), &special->power);
		params->image_width = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_imageWidth)));
		params->image_height = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_imageHeight)));
		params->doubles.imageAdjustments.brightness = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_brightness)), &special->brightness);
		params->doubles.imageAdjustments.imageGamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_gamma)), &special->imageGamma);
		params->doubles.imageAdjustments.ambient = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_ambient)), &special->ambient);
		params->doubles.imageAdjustments.globalIlum = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_ambient_occlusion)), &special->globalIlum);
		params->doubles.imageAdjustments.glow_intensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_glow)), &special->glow_intensity);
		params->doubles.imageAdjustments.reflect = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_reflect)), &special->reflect);
		params->doubles.imageAdjustments.shading = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_shading)), &special->shading);
		params->doubles.imageAdjustments.directLight = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_shadows)), &special->directLight);
		params->doubles.imageAdjustments.specular = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_specular)), &special->specular);
		params->global_ilumination = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkAmbientOcclusion));
		params->fastGlobalIllumination = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkFastAmbientOcclusion));
		params->globalIlumQuality = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_AmbientOcclusionQuality)));
		params->shadow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkShadow));
		params->iterThresh = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkIterThresh));
		params->juliaMode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkJulia));
		params->slowShading = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkSlowShading));
		params->textured_background = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkBitmapBackground));
		params->doubles.julia.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_julia_a)), &special->juliaX);
		params->doubles.julia.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_julia_b)), &special->juliaY);
		params->doubles.julia.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_julia_c)), &special->juliaZ);
		params->doubles.amin = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_amin)), &special->amin);
		params->doubles.amax = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_amax)), &special->amax);
		params->doubles.bmin = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_bmin)), &special->bmin);
		params->doubles.bmax = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_bmax)), &special->bmax);
		params->doubles.cmin = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_cmin)), &special->cmin);
		params->doubles.cmax = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_cmax)), &special->cmax);
		params->limits_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkLimits));
		params->imageSwitches.coloringEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkColoring));
		params->coloring_seed = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_color_seed)));
		params->doubles.imageAdjustments.coloring_speed = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_color_speed)), &special->coloring_speed);
		params->tgladFoldingMode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkTgladMode));
		params->sphericalFoldingMode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkSphericalFoldingMode));
		params->IFSFoldingMode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkIFSFoldingMode));
		params->doubles.foldingLimit = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_tglad_folding_1)), &special->foldingLimit);
		params->doubles.foldingValue = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_tglad_folding_2)), &special->foldingValue);
		params->doubles.foldingSphericalFixed = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_spherical_folding_1)), &special->foldingSphericalFixed);
		params->doubles.foldingSphericalMin = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_spherical_folding_2)), &special->foldingSphericalMin);
		params->imageSwitches.fogEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkFogEnabled));
		params->doubles.imageAdjustments.fogVisibility = gtk_adjustment_get_value(GTK_ADJUSTMENT(Interface.adjustmentFogDepth));
		params->doubles.imageAdjustments.fogVisibilityFront = gtk_adjustment_get_value(GTK_ADJUSTMENT(Interface.adjustmentFogDepthFront));
		params->SSAOQuality = gtk_adjustment_get_value(GTK_ADJUSTMENT(Interface.adjustmentSSAOQuality));
		params->SSAOEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkSSAOEnabled));
		params->DOFEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkDOFEnabled));
		params->doubles.DOFFocus = gtk_adjustment_get_value(GTK_ADJUSTMENT(Interface.adjustmentDOFFocus));
		params->doubles.DOFRadius = gtk_adjustment_get_value(GTK_ADJUSTMENT(Interface.adjustmentDOFRadius));
		params->doubles.imageAdjustments.mainLightIntensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mainLightIntensity)), &special->mainLightIntensity);
		params->doubles.auxLightIntensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightIntensity)), &special->auxLightIntensity);
		params->auxLightRandomSeed = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightRandomSeed)));
		params->auxLightNumber = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightNumber)));
		params->doubles.auxLightMaxDist = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightMaxDist)), &special->auxLightMaxDist);
		params->doubles.auxLightDistributionRadius = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightDistributionRadius)), &special->auxLightDistributionRadius);
		params->doubles.auxLightPre1.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre1x)), &special->auxLightPre1X);
		params->doubles.auxLightPre1.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre1y)), &special->auxLightPre1Y);
		params->doubles.auxLightPre1.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre1z)), &special->auxLightPre1Z);
		params->doubles.auxLightPre1intensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre1intensity)), &special->auxLightPre1intensity);
		params->doubles.auxLightPre2.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre2x)), &special->auxLightPre2X);
		params->doubles.auxLightPre2.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre2y)), &special->auxLightPre2Y);
		params->doubles.auxLightPre2.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre2z)), &special->auxLightPre2Z);
		params->doubles.auxLightPre2intensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre2intensity)), &special->auxLightPre2intensity);
		params->doubles.auxLightPre3.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre3x)), &special->auxLightPre3X);
		params->doubles.auxLightPre3.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre3y)), &special->auxLightPre3Y);
		params->doubles.auxLightPre3.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre3z)), &special->auxLightPre3Z);
		params->doubles.auxLightPre3intensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre3intensity)), &special->auxLightPre3intensity);
		params->doubles.auxLightPre4.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre4x)), &special->auxLightPre4X);
		params->doubles.auxLightPre4.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre4y)), &special->auxLightPre4Y);
		params->doubles.auxLightPre4.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre4z)), &special->auxLightPre4Z);
		params->doubles.auxLightPre4intensity = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightPre4intensity)), &special->auxLightPre4intensity);
		params->auxLightPre1Enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre1Enabled));
		params->auxLightPre2Enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre2Enabled));
		params->auxLightPre3Enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre3Enabled));
		params->auxLightPre4Enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre4Enabled));
		params->doubles.auxLightVisibility = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_auxLightVisibility)), &special->auxLightVisibility);
		params->doubles.mainLightAlfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mainLightAlfa)), &special->mainLightAlfa) / 180.0 * M_PI;
		params->doubles.mainLightBeta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mainLightBeta)), &special->mainLightBeta) / 180.0 * M_PI;
		params->doubles.IFSScale = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSScale)), &special->IFSScale);
		params->doubles.IFSRotationAlfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSAlfa)), &special->IFSRotationAlfa) / 180.0 * M_PI;
		params->doubles.IFSRotationBeta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSBeta)), &special->IFSRotationBeta) / 180.0 * M_PI;
		params->doubles.IFSRotationGamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSGamma)), &special->IFSRotationGamma) / 180.0 * M_PI;
		params->doubles.IFSOffset.x = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSOffsetX)), &special->IFSOffsetX);
		params->doubles.IFSOffset.y = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSOffsetY)), &special->IFSOffsetY);
		params->doubles.IFSOffset.z = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_IFSOffsetZ)), &special->IFSOffsetZ);
		params->IFSAbsX = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkIFSAbsX));
		params->IFSAbsY = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkIFSAbsY));
		params->IFSAbsZ = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkIFSAbsZ));
		params->startFrame = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_animationStartFrame)));
		params->endFrame = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_animationEndFrame)));
		params->framesPerKeyframe = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_animationFramesPerKey)));
		params->doubles.imageAdjustments.paletteOffset = gtk_adjustment_get_value(GTK_ADJUSTMENT(Interface.adjustmentPaletteOffset));
		params->soundBand1Min = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound1FreqMin)));
		params->soundBand1Max = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound1FreqMax)));
		params->soundBand2Min = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound2FreqMin)));
		params->soundBand2Max = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound2FreqMax)));
		params->soundBand3Min = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound3FreqMin)));
		params->soundBand3Max = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound3FreqMax)));
		params->soundBand4Min = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound4FreqMin)));
		params->soundBand4Max = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_sound4FreqMax)));
		params->soundEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkSoundEnabled));
		params->doubles.soundFPS = atof(gtk_entry_get_text(GTK_ENTRY(Interface.edit_soundFPS)));
		params->hybridIters1 = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridIter1)));
		params->hybridIters2 = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridIter2)));
		params->hybridIters3 = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridIter3)));
		params->hybridIters4 = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridIter4)));
		params->hybridIters5 = atoi(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridIter5)));
		params->doubles.hybridPower1 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridPower1)), &special->hybridPower1);
		params->doubles.hybridPower2 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridPower2)), &special->hybridPower2);
		params->doubles.hybridPower3 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridPower3)), &special->hybridPower3);
		params->doubles.hybridPower4 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridPower4)), &special->hybridPower4);
		params->doubles.hybridPower5 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_hybridPower5)), &special->hybridPower5);
		params->hybridCyclic = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkHybridCyclic));
		params->fishEye = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkFishEye));
		params->doubles.stereoEyeDistance = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_stereoDistance)), &special->stereoEyeDistance);
		params->stereoEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkStereoEnabled));

		params->mandelboxRotationsEnabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.checkMandelboxRotationsEnable));
		params->doubles.mandelboxFoldingLimit = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxFoldingLimit)), &special->mandelboxFoldingLimit);
		params->doubles.mandelboxFoldingValue = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxFoldingValue)), &special->mandelboxFoldingValue);
		params->doubles.mandelboxFoldingSphericalFixed = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxSpFoldingFixedRadius)), &special->mandelboxFoldingSphericalFixed);
		params->doubles.mandelboxFoldingSphericalMin = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxSpFoldingMinRadius)), &special->mandelboxFoldingSphericalMin);
		params->doubles.mandelboxRotationX1Alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationX1Alfa)), &special->mandelboxRotationX1Alfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationX1Beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationX1Beta)), &special->mandelboxRotationX1Beta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationX1Gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationX1Gamma)), &special->mandelboxRotationX1Gamma) / 180.0 * M_PI;
		params->doubles.mandelboxRotationX2Alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationX2Alfa)), &special->mandelboxRotationX2Alfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationX2Beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationX2Beta)), &special->mandelboxRotationX2Beta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationX2Gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationX2Gamma)), &special->mandelboxRotationX2Gamma) / 180.0 * M_PI;
		params->doubles.mandelboxRotationY1Alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationY1Alfa)), &special->mandelboxRotationY1Alfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationY1Beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationY1Beta)), &special->mandelboxRotationY1Beta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationY1Gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationY1Gamma)), &special->mandelboxRotationY1Gamma) / 180.0 * M_PI;
		params->doubles.mandelboxRotationY2Alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationY2Alfa)), &special->mandelboxRotationY2Alfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationY2Beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationY2Beta)), &special->mandelboxRotationY2Beta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationY2Gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationY2Gamma)), &special->mandelboxRotationY2Gamma) / 180.0 * M_PI;
		params->doubles.mandelboxRotationZ1Alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Alfa)), &special->mandelboxRotationZ1Alfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationZ1Beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Beta)), &special->mandelboxRotationZ1Beta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationZ1Gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Gamma)), &special->mandelboxRotationZ1Gamma) / 180.0 * M_PI;
		params->doubles.mandelboxRotationZ2Alfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Alfa)), &special->mandelboxRotationZ2Alfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationZ2Beta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Beta)), &special->mandelboxRotationZ2Beta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationZ2Gamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Gamma)), &special->mandelboxRotationZ2Gamma) / 180.0 * M_PI;
		params->doubles.mandelboxRotationMainAlfa = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationMainAlfa)), &special->mandelboxRotationMainAlfa) / 180.0 * M_PI;
		params->doubles.mandelboxRotationMainBeta = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationMainBeta)), &special->mandelboxRotationMainBeta) / 180.0 * M_PI;
		params->doubles.mandelboxRotationMainGamma = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxRotationMainGamma)), &special->mandelboxRotationMainGamma) / 180.0 * M_PI;
		params->doubles.mandelboxColorFactorR = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorR)), &special->mandelboxColorFactorR);
		params->doubles.mandelboxColorFactorX = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorX)), &special->mandelboxColorFactorX);
		params->doubles.mandelboxColorFactorY = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorY)), &special->mandelboxColorFactorY);
		params->doubles.mandelboxColorFactorZ = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorZ)), &special->mandelboxColorFactorZ);
		params->doubles.mandelboxColorFactorSp1 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorSp1)), &special->mandelboxColorFactorSp1);
		params->doubles.mandelboxColorFactorSp2 = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorSp2)), &special->mandelboxColorFactorSp2);
		params->doubles.mandelboxScale = atofData(gtk_entry_get_text(GTK_ENTRY(Interface.edit_mandelboxScale)), &special->mandelboxScale);

		params->image_width = (params->image_width/8)*8;
		params->image_height = (params->image_height/8)*8;

		special->paletteOffset.active = false;
		special->fogVisibility.active = false;
		special->fogVisibilityFront.active = false;
		special->DOFFocus.active = false;
		special->DOFRadius.active = false;

		for (int i = 0; i < IFS_number_of_vectors; i++)
		{
			params->IFSDirection[i].x = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSx)));
			params->IFSDirection[i].y = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSy)));
			params->IFSDirection[i].z = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSz)));
			params->IFSAlfa[i] = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSalfa))) / 180.0 * M_PI;
			params->IFSBeta[i] = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSbeta))) / 180.0 * M_PI;
			params->IFSGamma[i] = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSgamma))) / 180.0 * M_PI;
			params->IFSDistance[i] = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSdistance)));
			params->IFSIntensity[i] = atof(gtk_entry_get_text(GTK_ENTRY(Interface.IFSParams[i].editIFSintensity)));
			params->IFSEnabled[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Interface.IFSParams[i].checkIFSenabled));
		}

		GdkColor color;
		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorGlow1), &color);
		params->effectColours.glow_color1.R = color.red;
		params->effectColours.glow_color1.G = color.green;
		params->effectColours.glow_color1.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorGlow2), &color);
		params->effectColours.glow_color2.R = color.red;
		params->effectColours.glow_color2.G = color.green;
		params->effectColours.glow_color2.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorBackgroud1), &color);
		params->background_color1.R = color.red;
		params->background_color1.G = color.green;
		params->background_color1.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorBackgroud2), &color);
		params->background_color2.R = color.red;
		params->background_color2.G = color.green;
		params->background_color2.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorFog), &color);
		params->effectColours.fogColor.R = color.red;
		params->effectColours.fogColor.G = color.green;
		params->effectColours.fogColor.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre1), &color);
		params->auxLightPre1Colour.R = color.red;
		params->auxLightPre1Colour.G = color.green;
		params->auxLightPre1Colour.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre2), &color);
		params->auxLightPre2Colour.R = color.red;
		params->auxLightPre2Colour.G = color.green;
		params->auxLightPre2Colour.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre3), &color);
		params->auxLightPre3Colour.R = color.red;
		params->auxLightPre3Colour.G = color.green;
		params->auxLightPre3Colour.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre4), &color);
		params->auxLightPre4Colour.R = color.red;
		params->auxLightPre4Colour.G = color.green;
		params->auxLightPre4Colour.B = color.blue;

		gtk_color_button_get_color(GTK_COLOR_BUTTON(Interface.buColorMainLight), &color);
		params->effectColours.mainLightColour.R = color.red;
		params->effectColours.mainLightColour.G = color.green;
		params->effectColours.mainLightColour.B = color.blue;

		int formula = gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboFractType));
		if (formula == 0) params->formula = trig;
		if (formula == 1) params->formula = trig_DE;
		if (formula == 2) params->formula = fast_trig;
		if (formula == 3) params->formula = minus_fast_trig;
		if (formula == 4) params->formula = xenodreambuie;
		if (formula == 5) params->formula = hypercomplex;
		if (formula == 6) params->formula = quaternion;
		if (formula == 7) params->formula = menger_sponge;
		if (formula == 8) params->formula = tglad;
		if (formula == 9) params->formula = kaleidoscopic;
		if (formula == 10) params->formula = mandelbulb2;
		if (formula == 11) params->formula = mandelbulb3;
		if (formula == 12) params->formula = mandelbulb4;
		if (formula == 13) params->formula = hybrid;

		params->hybridFormula1 = FormulaNumberGUI2Data(gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboHybridFormula1)));
		params->hybridFormula2 = FormulaNumberGUI2Data(gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboHybridFormula2)));
		params->hybridFormula3 = FormulaNumberGUI2Data(gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboHybridFormula3)));
		params->hybridFormula4 = FormulaNumberGUI2Data(gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboHybridFormula4)));
		params->hybridFormula5 = FormulaNumberGUI2Data(gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboHybridFormula5)));

		double imageScale;
		int scale = gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.combo_imageScale));
		if (scale == 0) imageScale = 1.0/10.0;
		if (scale == 1) imageScale = 1.0/8.0;
		if (scale == 2) imageScale = 1.0/6.0;
		if (scale == 3) imageScale = 1.0/4.0;
		if (scale == 4) imageScale = 1.0/3.0;
		if (scale == 5) imageScale = 1.0/2.0;
		if (scale == 6) imageScale = 1.0;
		if (scale == 7) imageScale = 2.0;
		if (scale == 8) imageScale = 4.0;
		if (scale == 9) imageScale = 6.0;
		if (scale == 10) imageScale = 8.0;
		if (scale == 11)
		{
			int winWidth;
			int winHeight;
			gtk_window_get_size(GTK_WINDOW(window2),&winWidth,&winHeight);
			winWidth-=scrollbarSize;
			winHeight-=scrollbarSize;
			imageScale = (double)winWidth / params->image_width;
			if(params->image_height*imageScale > winHeight) imageScale = (double)winHeight / params->image_height;
		}
		Interface_data.imageScale = imageScale;

		params->imageFormat = gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboImageFormat));

		if (params->SSAOEnabled && params->global_ilumination)
		{
			GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window_interface), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
					"Warning! Both of Ambient occlusion based on simulated rays and Screen Space Ambient Occlusion (SSAO) are activated. SSAO will be deactivated");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			params->SSAOEnabled = false;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkSSAOEnabled), params->SSAOEnabled);
		}

		if (!params->SSAOEnabled && !params->global_ilumination && params->doubles.imageAdjustments.reflect > 0)
		{
			GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window_interface), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
					"Warning! For reflection effect the Screen Space Ambient Occlusion (SSAO) effect has to be activated");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			params->SSAOEnabled = true;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkSSAOEnabled), params->SSAOEnabled);
		}

	}
	if (params->formula == trig_DE || params->formula == menger_sponge || params->formula == kaleidoscopic || params->formula == tglad) params->analitycDE = true;
	else params->analitycDE = false;
	params->doubles.max_y = 20.0 / params->doubles.zoom;
	params->doubles.resolution = 1.0 / params->image_width;

	InterfaceData2Params(params);

	Interface_data.imageFormat = (enumImageFormat) params->imageFormat;

	mainImage->SetImageParameters(params->doubles.imageAdjustments,params->effectColours,params->imageSwitches);

	//srand(Interface_data.coloring_seed);
	//NowaPaleta(paleta, 1.0);

	sRGB *palette2 = mainImage->GetPalettePtr();
	for (int i = 0; i < 256; i++)
	{
		params->palette[i] = palette2[i];
	}

	RecalculateIFSParams(params);
	CreateFormulaSequence(params);

	if (freeSpecial) delete special;
}

char* DoubleToString(double value, sAddData *addData)
{
	static char text[100];
	sprintf(text, "%.16lg", value);

	if (addData != 0 && addData->active == true)
	{
		char *symbol;
		if (addData->mode == soundEnvelope) symbol = (char*) "s0";
		if (addData->mode == soundA) symbol = (char*) "sa";
		if (addData->mode == soundB) symbol = (char*) "sb";
		if (addData->mode == soundC) symbol = (char*) "sc";
		if (addData->mode == soundD) symbol = (char*) "sd";
		sprintf(text, "%s %.16lg", symbol, addData->amp);
	}

	return text;
}

char*
IntToString(int value)
{
	static char text[100];
	sprintf(text, "%d", value);
	return text;
}

void WriteInterface(sParamRender *params, sParamSpecial *special)
{
	bool freeSpecial = false;
	if (special == 0)
	{
		special = new sParamSpecial;
		freeSpecial = true;
		memset(special, 0, sizeof(sParamSpecial));
	}

	gtk_entry_set_text(GTK_ENTRY(Interface.edit_va), DoubleToString(params->doubles.vp.x, &special->vpX));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_vb), DoubleToString(params->doubles.vp.y, &special->vpY));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_vc), DoubleToString(params->doubles.vp.z, &special->vpZ));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_alfa), DoubleToString(params->doubles.alfa * 180.0 / M_PI, &special->alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_beta), DoubleToString(params->doubles.beta * 180.0 / M_PI, &special->beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_gammaAngle), DoubleToString(params->doubles.gamma * 180.0 / M_PI, &special->gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_zoom), DoubleToString(params->doubles.zoom, &special->zoom));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_persp), DoubleToString(params->doubles.persp, &special->persp));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_DE_stepFactor), DoubleToString(params->doubles.DE_factor, &special->DE_factor));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_DE_thresh), DoubleToString(params->doubles.quality, &special->quality));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_maxN), IntToString(params->N));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_minN), IntToString(params->minN));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_power), DoubleToString(params->doubles.power, &special->power));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_imageWidth), IntToString(params->image_width));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_imageHeight), IntToString(params->image_height));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_brightness), DoubleToString(params->doubles.imageAdjustments.brightness, &special->brightness));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_gamma), DoubleToString(params->doubles.imageAdjustments.imageGamma, &special->imageGamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_ambient), DoubleToString(params->doubles.imageAdjustments.ambient, &special->ambient));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_ambient_occlusion), DoubleToString(params->doubles.imageAdjustments.globalIlum, &special->globalIlum));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_glow), DoubleToString(params->doubles.imageAdjustments.glow_intensity, &special->glow_intensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_reflect), DoubleToString(params->doubles.imageAdjustments.reflect, &special->reflect));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_shading), DoubleToString(params->doubles.imageAdjustments.shading, &special->shading));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_shadows), DoubleToString(params->doubles.imageAdjustments.directLight, &special->directLight));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_specular), DoubleToString(params->doubles.imageAdjustments.specular, &special->specular));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_AmbientOcclusionQuality), IntToString(params->globalIlumQuality));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_julia_a), DoubleToString(params->doubles.julia.x, &special->juliaX));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_julia_b), DoubleToString(params->doubles.julia.y, &special->juliaY));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_julia_c), DoubleToString(params->doubles.julia.z, &special->juliaZ));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_amin), DoubleToString(params->doubles.amin, &special->amin));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_amax), DoubleToString(params->doubles.amax, &special->amax));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_bmin), DoubleToString(params->doubles.bmin, &special->bmin));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_bmax), DoubleToString(params->doubles.bmax, &special->bmax));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_cmin), DoubleToString(params->doubles.cmin, &special->cmin));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_cmax), DoubleToString(params->doubles.cmax, &special->cmax));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_color_seed), IntToString(params->coloring_seed));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_color_speed), DoubleToString(params->doubles.imageAdjustments.coloring_speed, &special->coloring_speed));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_tglad_folding_1), DoubleToString(params->doubles.foldingLimit, &special->foldingLimit));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_tglad_folding_2), DoubleToString(params->doubles.foldingValue, &special->foldingValue));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_spherical_folding_1), DoubleToString(params->doubles.foldingSphericalFixed, &special->foldingSphericalFixed));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_spherical_folding_2), DoubleToString(params->doubles.foldingSphericalMin, &special->foldingSphericalMin));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mainLightIntensity), DoubleToString(params->doubles.imageAdjustments.mainLightIntensity, &special->mainLightIntensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightIntensity), DoubleToString(params->doubles.auxLightIntensity, &special->auxLightIntensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightRandomSeed), IntToString(params->auxLightRandomSeed));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightNumber), IntToString(params->auxLightNumber));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightMaxDist), DoubleToString(params->doubles.auxLightMaxDist, &special->auxLightMaxDist));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightDistributionRadius), DoubleToString(params->doubles.auxLightDistributionRadius, &special->auxLightDistributionRadius));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre1x), DoubleToString(params->doubles.auxLightPre1.x, &special->auxLightPre1X));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre1y), DoubleToString(params->doubles.auxLightPre1.y, &special->auxLightPre1Y));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre1z), DoubleToString(params->doubles.auxLightPre1.z, &special->auxLightPre1Z));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre1intensity), DoubleToString(params->doubles.auxLightPre1intensity, &special->auxLightPre1intensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre2x), DoubleToString(params->doubles.auxLightPre2.x, &special->auxLightPre2X));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre2y), DoubleToString(params->doubles.auxLightPre2.y, &special->auxLightPre2Y));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre2z), DoubleToString(params->doubles.auxLightPre2.z, &special->auxLightPre2Z));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre2intensity), DoubleToString(params->doubles.auxLightPre2intensity, &special->auxLightPre2intensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre3x), DoubleToString(params->doubles.auxLightPre3.x, &special->auxLightPre3X));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre3y), DoubleToString(params->doubles.auxLightPre3.y, &special->auxLightPre3Y));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre3z), DoubleToString(params->doubles.auxLightPre3.z, &special->auxLightPre3Z));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre3intensity), DoubleToString(params->doubles.auxLightPre3intensity, &special->auxLightPre3intensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre4x), DoubleToString(params->doubles.auxLightPre4.x, &special->auxLightPre4X));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre4y), DoubleToString(params->doubles.auxLightPre4.y, &special->auxLightPre4Y));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre4z), DoubleToString(params->doubles.auxLightPre4.z, &special->auxLightPre4Z));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightPre4intensity), DoubleToString(params->doubles.auxLightPre4intensity, &special->auxLightPre4intensity));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mainLightAlfa), DoubleToString(params->doubles.mainLightAlfa * 180.0 / M_PI, &special->mainLightAlfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mainLightBeta), DoubleToString(params->doubles.mainLightBeta * 180.0 / M_PI, &special->mainLightBeta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_auxLightVisibility), DoubleToString(params->doubles.auxLightVisibility, &special->auxLightVisibility));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSScale), DoubleToString(params->doubles.IFSScale, &special->IFSScale));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSAlfa), DoubleToString(params->doubles.IFSRotationAlfa * 180.0 / M_PI, &special->IFSRotationAlfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSBeta), DoubleToString(params->doubles.IFSRotationBeta * 180.0 / M_PI, &special->IFSRotationBeta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSGamma), DoubleToString(params->doubles.IFSRotationGamma * 180.0 / M_PI, &special->IFSRotationGamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSOffsetX), DoubleToString(params->doubles.IFSOffset.x, &special->IFSOffsetX));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSOffsetY), DoubleToString(params->doubles.IFSOffset.y, &special->IFSOffsetY));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_IFSOffsetZ), DoubleToString(params->doubles.IFSOffset.z, &special->IFSOffsetZ));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_animationStartFrame), IntToString(params->startFrame));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_animationEndFrame), IntToString(params->endFrame));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_animationFramesPerKey), IntToString(params->framesPerKeyframe));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_soundFPS), DoubleToString(params->doubles.soundFPS));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound1FreqMin), IntToString(params->soundBand1Min));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound1FreqMax), IntToString(params->soundBand1Max));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound2FreqMin), IntToString(params->soundBand2Min));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound2FreqMax), IntToString(params->soundBand2Max));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound3FreqMin), IntToString(params->soundBand3Min));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound3FreqMax), IntToString(params->soundBand3Max));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound4FreqMin), IntToString(params->soundBand4Min));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_sound4FreqMax), IntToString(params->soundBand4Max));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridPower1), DoubleToString(params->doubles.hybridPower1, &special->hybridPower1));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridPower2), DoubleToString(params->doubles.hybridPower2, &special->hybridPower2));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridPower3), DoubleToString(params->doubles.hybridPower3, &special->hybridPower3));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridPower4), DoubleToString(params->doubles.hybridPower4, &special->hybridPower4));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridPower5), DoubleToString(params->doubles.hybridPower5, &special->hybridPower5));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridIter1), IntToString(params->hybridIters1));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridIter2), IntToString(params->hybridIters2));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridIter3), IntToString(params->hybridIters3));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridIter4), IntToString(params->hybridIters4));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_hybridIter5), IntToString(params->hybridIters5));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_stereoDistance), DoubleToString(params->doubles.stereoEyeDistance, &special->stereoEyeDistance));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorR), DoubleToString(params->doubles.mandelboxColorFactorR, &special->mandelboxColorFactorR));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorX), DoubleToString(params->doubles.mandelboxColorFactorX, &special->mandelboxColorFactorX));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorY), DoubleToString(params->doubles.mandelboxColorFactorY, &special->mandelboxColorFactorY));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorZ), DoubleToString(params->doubles.mandelboxColorFactorZ, &special->mandelboxColorFactorZ));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorSp1), DoubleToString(params->doubles.mandelboxColorFactorSp1, &special->mandelboxColorFactorSp1));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxColorFactorSp2), DoubleToString(params->doubles.mandelboxColorFactorSp2, &special->mandelboxColorFactorSp2));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxFoldingLimit), DoubleToString(params->doubles.mandelboxFoldingLimit, &special->mandelboxFoldingLimit));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxFoldingValue), DoubleToString(params->doubles.mandelboxFoldingValue, &special->mandelboxFoldingLimit));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxSpFoldingFixedRadius), DoubleToString(params->doubles.mandelboxFoldingSphericalFixed, &special->mandelboxFoldingSphericalFixed));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxSpFoldingMinRadius), DoubleToString(params->doubles.mandelboxFoldingSphericalMin, &special->mandelboxFoldingSphericalMin));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationMainAlfa), DoubleToString(params->doubles.mandelboxRotationMainAlfa * 180.0 / M_PI, &special->mandelboxRotationMainAlfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationMainBeta), DoubleToString(params->doubles.mandelboxRotationMainBeta * 180.0 / M_PI, &special->mandelboxRotationMainBeta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationMainGamma), DoubleToString(params->doubles.mandelboxRotationMainGamma * 180.0 / M_PI, &special->mandelboxRotationMainGamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationX1Alfa), DoubleToString(params->doubles.mandelboxRotationX1Alfa * 180.0 / M_PI, &special->mandelboxRotationX1Alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationX1Beta), DoubleToString(params->doubles.mandelboxRotationX1Beta * 180.0 / M_PI, &special->mandelboxRotationX1Beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationX1Gamma), DoubleToString(params->doubles.mandelboxRotationX1Gamma * 180.0 / M_PI, &special->mandelboxRotationX1Gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationX2Alfa), DoubleToString(params->doubles.mandelboxRotationX2Alfa * 180.0 / M_PI, &special->mandelboxRotationX2Alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationX2Beta), DoubleToString(params->doubles.mandelboxRotationX2Beta * 180.0 / M_PI, &special->mandelboxRotationX2Beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationX2Gamma), DoubleToString(params->doubles.mandelboxRotationX2Gamma * 180.0 / M_PI, &special->mandelboxRotationX2Gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationY1Alfa), DoubleToString(params->doubles.mandelboxRotationY1Alfa * 180.0 / M_PI, &special->mandelboxRotationY1Alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationY1Beta), DoubleToString(params->doubles.mandelboxRotationY1Beta * 180.0 / M_PI, &special->mandelboxRotationY1Beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationY1Gamma), DoubleToString(params->doubles.mandelboxRotationY1Gamma * 180.0 / M_PI, &special->mandelboxRotationY1Gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationY2Alfa), DoubleToString(params->doubles.mandelboxRotationY2Alfa * 180.0 / M_PI, &special->mandelboxRotationY2Alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationY2Beta), DoubleToString(params->doubles.mandelboxRotationY2Beta * 180.0 / M_PI, &special->mandelboxRotationY2Beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationY2Gamma), DoubleToString(params->doubles.mandelboxRotationY2Gamma * 180.0 / M_PI, &special->mandelboxRotationY2Gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Alfa), DoubleToString(params->doubles.mandelboxRotationZ1Alfa * 180.0 / M_PI, &special->mandelboxRotationZ1Alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Beta), DoubleToString(params->doubles.mandelboxRotationZ1Beta * 180.0 / M_PI, &special->mandelboxRotationZ1Beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Gamma), DoubleToString(params->doubles.mandelboxRotationZ1Gamma * 180.0 / M_PI, &special->mandelboxRotationZ1Gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Alfa), DoubleToString(params->doubles.mandelboxRotationZ2Alfa * 180.0 / M_PI, &special->mandelboxRotationZ2Alfa));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Beta), DoubleToString(params->doubles.mandelboxRotationZ2Beta * 180.0 / M_PI, &special->mandelboxRotationZ2Beta));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Gamma), DoubleToString(params->doubles.mandelboxRotationZ2Gamma * 180.0 / M_PI, &special->mandelboxRotationZ2Gamma));
	gtk_entry_set_text(GTK_ENTRY(Interface.edit_mandelboxScale), DoubleToString(params->doubles.mandelboxScale, &special->mandelboxScale));

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkAmbientOcclusion), params->global_ilumination);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkFastAmbientOcclusion), params->fastGlobalIllumination);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkShadow), params->shadow);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkIterThresh), params->iterThresh);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkJulia), params->juliaMode);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkSlowShading), params->slowShading);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkLimits), params->limits_enabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkBitmapBackground), params->textured_background);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkColoring), params->imageSwitches.coloringEnabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkTgladMode), params->tgladFoldingMode);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkSphericalFoldingMode), params->sphericalFoldingMode);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkIFSFoldingMode), params->IFSFoldingMode);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkFogEnabled), params->imageSwitches.fogEnabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkSSAOEnabled), params->SSAOEnabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkDOFEnabled), params->DOFEnabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre1Enabled), params->auxLightPre1Enabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre2Enabled), params->auxLightPre2Enabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre3Enabled), params->auxLightPre3Enabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkAuxLightPre4Enabled), params->auxLightPre4Enabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkIFSAbsX), params->IFSAbsX);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkIFSAbsY), params->IFSAbsY);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkIFSAbsZ), params->IFSAbsZ);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkSoundEnabled), params->soundEnabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkHybridCyclic), params->hybridCyclic);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkFishEye), params->fishEye);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkStereoEnabled), params->stereoEnabled);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkMandelboxRotationsEnable), params->mandelboxRotationsEnabled);

	gtk_adjustment_set_value(GTK_ADJUSTMENT(Interface.adjustmentFogDepth), params->doubles.imageAdjustments.fogVisibility);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(Interface.adjustmentFogDepthFront), params->doubles.imageAdjustments.fogVisibilityFront);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(Interface.adjustmentSSAOQuality), params->SSAOQuality);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(Interface.adjustmentDOFFocus), params->doubles.DOFFocus);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(Interface.adjustmentDOFRadius), params->doubles.DOFRadius);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(Interface.adjustmentPaletteOffset), params->doubles.imageAdjustments.paletteOffset);

	for (int i = 0; i < IFS_number_of_vectors; i++)
	{
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSx), DoubleToString(params->IFSDirection[i].x));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSy), DoubleToString(params->IFSDirection[i].y));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSz), DoubleToString(params->IFSDirection[i].z));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSalfa), DoubleToString(params->IFSAlfa[i] * 180.0 / M_PI));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSbeta), DoubleToString(params->IFSBeta[i] * 180.0 / M_PI));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSgamma), DoubleToString(params->IFSGamma[i] * 180.0 / M_PI));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSdistance), DoubleToString(params->IFSDistance[i]));
		gtk_entry_set_text(GTK_ENTRY(Interface.IFSParams[i].editIFSintensity), DoubleToString(params->IFSIntensity[i]));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.IFSParams[i].checkIFSenabled), params->IFSEnabled[i]);
	}

	int formula = gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboFractType));
	if (params->formula == trig) formula = 0;
	if (params->formula == trig_DE) formula = 1;
	if (params->formula == fast_trig) formula = 2;
	if (params->formula == minus_fast_trig) formula = 3;
	if (params->formula == xenodreambuie) formula = 4;
	if (params->formula == hypercomplex) formula = 5;
	if (params->formula == quaternion) formula = 6;
	if (params->formula == menger_sponge) formula = 7;
	if (params->formula == tglad) formula = 8;
	if (params->formula == kaleidoscopic) formula = 9;
	if (params->formula == mandelbulb2) formula = 10;
	if (params->formula == mandelbulb3) formula = 11;
	if (params->formula == mandelbulb4) formula = 12;
	if (params->formula == hybrid) formula = 13;
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboFractType), formula);

	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboHybridFormula1), FormulaNumberData2GUI(params->hybridFormula1));
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboHybridFormula2), FormulaNumberData2GUI(params->hybridFormula2));
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboHybridFormula3), FormulaNumberData2GUI(params->hybridFormula3));
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboHybridFormula4), FormulaNumberData2GUI(params->hybridFormula4));
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboHybridFormula5), FormulaNumberData2GUI(params->hybridFormula5));

	GdkColor color;

	color.red = params->effectColours.glow_color1.R;
	color.green = params->effectColours.glow_color1.G;
	color.blue = params->effectColours.glow_color1.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorGlow1), &color);

	color.red = params->effectColours.glow_color2.R;
	color.green = params->effectColours.glow_color2.G;
	color.blue = params->effectColours.glow_color2.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorGlow2), &color);

	color.red = params->background_color1.R;
	color.green = params->background_color1.G;
	color.blue = params->background_color1.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorBackgroud1), &color);

	color.red = params->background_color2.R;
	color.green = params->background_color2.G;
	color.blue = params->background_color2.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorBackgroud2), &color);

	color.red = params->effectColours.fogColor.R;
	color.green = params->effectColours.fogColor.G;
	color.blue = params->effectColours.fogColor.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorFog), &color);

	color.red = params->auxLightPre1Colour.R;
	color.green = params->auxLightPre1Colour.G;
	color.blue = params->auxLightPre1Colour.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre1), &color);

	color.red = params->auxLightPre2Colour.R;
	color.green = params->auxLightPre2Colour.G;
	color.blue = params->auxLightPre2Colour.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre2), &color);

	color.red = params->auxLightPre3Colour.R;
	color.green = params->auxLightPre3Colour.G;
	color.blue = params->auxLightPre3Colour.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre3), &color);

	color.red = params->auxLightPre4Colour.R;
	color.green = params->auxLightPre4Colour.G;
	color.blue = params->auxLightPre4Colour.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre4), &color);

	color.red = params->effectColours.mainLightColour.R;
	color.green = params->effectColours.mainLightColour.G;
	color.blue = params->effectColours.mainLightColour.B;
	gtk_color_button_set_color(GTK_COLOR_BUTTON(Interface.buColorMainLight), &color);

	DrawPalette(params->palette);

	if (freeSpecial) delete special;
}

void AddComboTextsFractalFormula(GtkComboBox *combo)
{
	gtk_combo_box_append_text(combo, "None");
	gtk_combo_box_append_text(combo, "Trigonometric");
	gtk_combo_box_append_text(combo, "Polynomic power 2");
	gtk_combo_box_append_text(combo, "Polynomic power 2 - minus z");
	gtk_combo_box_append_text(combo, "Xenodreambuie's formula");
	gtk_combo_box_append_text(combo, "Hypercomplex");
	gtk_combo_box_append_text(combo, "Quaternion");
	gtk_combo_box_append_text(combo, "Menger sponge");
	gtk_combo_box_append_text(combo, "Mandelbox");
	gtk_combo_box_append_text(combo, "Kaleidoscopic IFS");
	gtk_combo_box_append_text(combo, "Modified Mandelbulb 1");
	gtk_combo_box_append_text(combo, "Modified Mandelbulb 2");
	gtk_combo_box_append_text(combo, "Modified Mandelbulb 3");
}

void CreateInterface(sParamRender *default_settings)
{
	//------------- glowne okno renderowania
	window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window2), "Mandelbulb Render Window");
	g_signal_connect(G_OBJECT(window2), "delete_event", G_CALLBACK(StopRenderingAndQuit), NULL);
	gtk_widget_add_events(GTK_WIDGET(window2), GDK_CONFIGURE);
  g_signal_connect(G_OBJECT(window2), "configure-event", G_CALLBACK(WindowReconfigured), NULL);

	//glowny box w oknie
	GtkWidget *box1 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(box1), 0);

	//obszar rysowania
	darea = gtk_drawing_area_new();

	gtk_widget_set_size_request(darea, mainImage->GetPreviewWidth(), mainImage->GetPreviewHeight());
	gtk_window_set_default_size(GTK_WINDOW(window2), mainImage->GetPreviewWidth() + 16, mainImage->GetPreviewHeight() + 16);
	lastWindowWidth = mainImage->GetPreviewWidth()+16;
	lastWindowHeight = mainImage->GetPreviewHeight()+16;

	gtk_signal_connect(GTK_OBJECT(darea), "expose-event", GTK_SIGNAL_FUNC(on_darea_expose), NULL);
	gtk_signal_connect(GTK_OBJECT(darea), "motion_notify_event", (GtkSignalFunc) motion_notify_event, NULL);
	gtk_signal_connect(GTK_OBJECT(darea), "button_press_event", GTK_SIGNAL_FUNC(pressed_button_on_image), NULL);
	gtk_widget_set_events(darea, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);

	GtkObject *hadjustment = gtk_adjustment_new(0, 0, 1000, 1, 1, 100);
	GtkObject *vadjustment = gtk_adjustment_new(0, 0, 1000, 1, 1, 100);

	GtkWidget *scrolled_window = gtk_scrolled_window_new(GTK_ADJUSTMENT(hadjustment), GTK_ADJUSTMENT(vadjustment));

	//wstawienie darea do box
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), darea);

	//wstawienie box do window
	gtk_container_add(GTK_CONTAINER(window2), scrolled_window);

	//wyswietlenie wszystkich widgetow
	gtk_widget_show_all(window2);

	//get scrollbar size
	GtkWidget *hscrollbar = gtk_scrolled_window_get_hscrollbar(GTK_SCROLLED_WINDOW(scrolled_window));
	scrollbarSize = hscrollbar->allocation.height;

	//------------------- okno histogramu iteracji ------------

	window_histogram = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_histogram), "left - number of iterations (max 64) / right - number of steps (max. 1000)");
	g_signal_connect(G_OBJECT(window_histogram), "delete_event", G_CALLBACK(StopRenderingAndQuit), NULL);

	//glowny box w oknie
	GtkWidget *box2 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(box2), 5);

	//obszar rysowania
	darea2 = gtk_drawing_area_new();
	gtk_widget_set_size_request(darea2, 512, 128);

	gtk_box_pack_start(GTK_BOX(box2), darea2, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window_histogram), box2);
	gtk_widget_show_all(window_histogram);

	//------------------- MAIN WINDOW ------------

	window_interface = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_interface), "Mandelbulber (default.fract)");
	gtk_container_set_border_width(GTK_CONTAINER(window_interface), 10);
	g_signal_connect(G_OBJECT(window_interface), "delete_event", G_CALLBACK(StopRenderingAndQuit), NULL);

	//tabs
	Interface.tabs = gtk_notebook_new();

	Interface.tab_label_view = gtk_label_new("View");
	Interface.tab_label_fractal = gtk_label_new("Fractal");
	Interface.tab_label_hybrid = gtk_label_new("Hybrid");
	Interface.tab_label_mandelbox = gtk_label_new("Mandelbox");
	Interface.tab_label_shaders = gtk_label_new("Shaders");
	Interface.tab_label_image = gtk_label_new("Image");
	Interface.tab_label_animation = gtk_label_new("Animation");
	Interface.tab_label_posteffects = gtk_label_new("Post effects");
	Interface.tab_label_lights = gtk_label_new("Lights");
	Interface.tab_label_IFS = gtk_label_new("IFS");
	Interface.tab_label_sound = gtk_label_new("Sound");
	Interface.tab_label_about = gtk_label_new("About...");

	Interface.tab_box_view = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_fractal = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_shaders = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_image = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_animation = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_posteffects = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_about = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_lights = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_sound = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_IFS = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_hybrid = gtk_vbox_new(FALSE, 1);
	Interface.tab_box_mandelbox = gtk_vbox_new(FALSE, 1);

	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_view), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_fractal), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_shaders), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_image), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_animation), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_posteffects), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_lights), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_IFS), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_sound), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_about), 5);
	gtk_container_set_border_width(GTK_CONTAINER(Interface.tab_box_hybrid), 5);

	//boxes
	Interface.boxMain = gtk_vbox_new(FALSE, 1);
	Interface.boxButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxView = gtk_vbox_new(FALSE, 1);
	Interface.boxCoordinates = gtk_hbox_new(FALSE, 1);
	Interface.boxAngle = gtk_hbox_new(FALSE, 1);
	Interface.boxNavigation = gtk_vbox_new(FALSE, 1);
	Interface.boxNavigationButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxNavigationZooming = gtk_hbox_new(FALSE, 1);
	Interface.boxZoom = gtk_hbox_new(FALSE, 1);
	Interface.boxArrows = gtk_hbox_new(FALSE, 1);
	Interface.boxArrows2 = gtk_vbox_new(FALSE, 1);
	Interface.boxArrows3 = gtk_vbox_new(FALSE, 1);
	Interface.boxFractal = gtk_vbox_new(FALSE, 1);
	Interface.boxLimits = gtk_hbox_new(FALSE, 1);
	Interface.boxJulia = gtk_hbox_new(FALSE, 1);
	Interface.boxQuality = gtk_hbox_new(FALSE, 1);
	Interface.boxImage = gtk_vbox_new(FALSE, 1);
	Interface.boxImageRes = gtk_hbox_new(FALSE, 1);
	Interface.boxEffects = gtk_vbox_new(FALSE, 1);
	Interface.boxBrightness = gtk_hbox_new(FALSE, 1);
	Interface.boxShading = gtk_hbox_new(FALSE, 1);
	Interface.boxShading2 = gtk_hbox_new(FALSE, 1);
	Interface.boxEffectsChecks = gtk_hbox_new(FALSE, 1);
	Interface.boxEffectsChecks2 = gtk_hbox_new(FALSE, 1);
	Interface.boxEffectsColoring = gtk_hbox_new(FALSE, 1);
	Interface.boxColors = gtk_vbox_new(FALSE, 1);
	Interface.boxGlowColor = gtk_hbox_new(FALSE, 1);
	Interface.boxLoadSave = gtk_hbox_new(FALSE, 1);
	Interface.boxAnimation = gtk_vbox_new(FALSE, 1);
	Interface.boxAnimationButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxAnimationEdits = gtk_hbox_new(FALSE, 1);
	Interface.boxAnimationEdits2 = gtk_hbox_new(FALSE, 1);
	Interface.boxTgladFolding = gtk_hbox_new(FALSE, 1);
	Interface.boxSphericalFolding = gtk_hbox_new(FALSE, 1);
	Interface.boxSaveImage = gtk_hbox_new(FALSE, 1);
	Interface.boxPostFog = gtk_vbox_new(FALSE, 1);
	Interface.boxFogButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxFogSlider = gtk_hbox_new(FALSE, 1);
	Interface.boxFogSlider2 = gtk_hbox_new(FALSE, 1);
	Interface.boxPostSSAO = gtk_vbox_new(FALSE, 1);
	Interface.boxSSAOButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxSSAOSlider = gtk_hbox_new(FALSE, 1);
	Interface.boxPostDOF = gtk_vbox_new(FALSE, 1);
	Interface.boxDOFSlider1 = gtk_hbox_new(FALSE, 1);
	Interface.boxDOFSlider2 = gtk_hbox_new(FALSE, 1);
	Interface.boxDOFButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxLightBallance = gtk_vbox_new(FALSE, 1);
	Interface.boxLightsParameters = gtk_vbox_new(FALSE, 1);
	Interface.boxPredefinedLights = gtk_vbox_new(FALSE, 1);
	Interface.boxLightBrightness = gtk_hbox_new(FALSE, 1);
	Interface.boxLightDistribution = gtk_hbox_new(FALSE, 1);
	Interface.boxLightDistribution2 = gtk_hbox_new(FALSE, 1);
	Interface.boxLightPre1 = gtk_hbox_new(FALSE, 1);
	Interface.boxLightPre2 = gtk_hbox_new(FALSE, 1);
	Interface.boxLightPre3 = gtk_hbox_new(FALSE, 1);
	Interface.boxLightPre4 = gtk_hbox_new(FALSE, 1);
	Interface.boxMainLight = gtk_vbox_new(FALSE, 1);
	Interface.boxMainLightPosition = gtk_hbox_new(FALSE, 1);
	Interface.boxIFSMain = gtk_vbox_new(FALSE, 1);
	Interface.boxIFSMainEdit = gtk_hbox_new(FALSE, 1);
	Interface.boxIFSMainEdit2 = gtk_hbox_new(FALSE, 1);
	Interface.boxIFSParams = gtk_vbox_new(FALSE, 1);
	Interface.boxIFSButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxKeyframeAnimation = gtk_vbox_new(FALSE, 1);
	Interface.boxKeyframeAnimationButtons = gtk_hbox_new(FALSE, 1);
	Interface.boxKeyframeAnimationButtons2 = gtk_hbox_new(FALSE, 1);
	Interface.boxKeyframeAnimationEdits = gtk_hbox_new(FALSE, 1);
	Interface.boxBottomKeyframeAnimation = gtk_hbox_new(FALSE, 1);
	Interface.boxPalette = gtk_vbox_new(FALSE, 1);
	Interface.boxPaletteOffset = gtk_hbox_new(FALSE, 1);
	Interface.boxImageSaving = gtk_vbox_new(FALSE, 1);
	Interface.boxImageAutoSave = gtk_hbox_new(FALSE, 1);
	Interface.boxSound = gtk_vbox_new(FALSE, 1);
	Interface.boxSoundBand1 = gtk_hbox_new(FALSE, 1);
	Interface.boxSoundBand2 = gtk_hbox_new(FALSE, 1);
	Interface.boxSoundBand3 = gtk_hbox_new(FALSE, 1);
	Interface.boxSoundBand4 = gtk_hbox_new(FALSE, 1);
	Interface.boxSoundMisc = gtk_hbox_new(FALSE, 1);
	Interface.boxHybrid = gtk_vbox_new(FALSE, 1);
	Interface.boxStereoscopic = gtk_vbox_new(FALSE, 1);
	Interface.boxStereoParams = gtk_hbox_new(FALSE, 1);
	Interface.boxMandelboxMainParams = gtk_vbox_new(FALSE, 1);
	Interface.boxMandelboxRotations = gtk_vbox_new(FALSE, 1);
	Interface.boxMandelboxColoring = gtk_vbox_new(FALSE, 1);
	Interface.boxMandelboxMainParams1 = gtk_hbox_new(FALSE, 1);
	Interface.boxMandelboxMainParams2 = gtk_hbox_new(FALSE, 1);
	Interface.boxMandelboxRotationMain = gtk_hbox_new(FALSE, 1);
	Interface.boxMandelboxColor1 = gtk_hbox_new(FALSE, 1);
	Interface.boxMandelboxColor2 = gtk_hbox_new(FALSE, 1);
	Interface.boxMandelboxColor3 = gtk_hbox_new(FALSE, 1);

	//tables
	Interface.tableLimits = gtk_table_new(2, 3, false);
	Interface.tableArrows = gtk_table_new(3, 3, false);
	Interface.tableArrows2 = gtk_table_new(3, 3, false);
	Interface.tableIFSParams = gtk_table_new(9, 9, false);
	Interface.tableHybridParams = gtk_table_new(5, 3, false);
	Interface.tableMandelboxRotations = gtk_table_new(5, 7, false);

	//frames
	Interface.frCoordinates = gtk_frame_new("Viewpoint coordinates");
	Interface.fr3Dnavigator = gtk_frame_new("3D Navigator");
	Interface.frFractal = gtk_frame_new("Fractal Parameters");
	Interface.frLimits = gtk_frame_new("Limits");
	Interface.frImage = gtk_frame_new("Image parameters");
	Interface.frEffects = gtk_frame_new("Shading effects");
	Interface.frColors = gtk_frame_new("Colors");
	Interface.frLoadSave = gtk_frame_new("Settings");
	Interface.frAnimation = gtk_frame_new("Flight animation");
	Interface.frAnimationFrames = gtk_frame_new("Frames to render");
	Interface.frPostFog = gtk_frame_new("Fog");
	Interface.frPostSSAO = gtk_frame_new("Screen space ambient occlusion");
	Interface.frPostDOF = gtk_frame_new("Depth of field");
	Interface.frLightBallance = gtk_frame_new("Light brightness ballance");
	Interface.frLightsParameters = gtk_frame_new("Random lights parameters");
	Interface.frPredefinedLights = gtk_frame_new("Predefined Lights");
	Interface.frMainLight = gtk_frame_new("Main light source (connected with camera)");
	Interface.frIFSMain = gtk_frame_new("General IFS parameters");
	Interface.frIFSParams = gtk_frame_new("Symetry vectors");
	Interface.frKeyframeAnimation = gtk_frame_new("Keyframe animation");
	Interface.frKeyframeAnimation2 = gtk_frame_new("Key-frames");
	Interface.frPalette = gtk_frame_new("Colour palette");
	Interface.frImageSaving = gtk_frame_new("Image saving");
	Interface.frSound = gtk_frame_new("Animation by sound");
	Interface.frHybrid = gtk_frame_new("Hybrid formula");
	Interface.frStereo = gtk_frame_new("Stereoscopic rendering");
	Interface.frMandelboxMainParams = gtk_frame_new("Main Mandelbox parameters");
	Interface.frMandelboxRotations = gtk_frame_new("Rotation of Mandelbox folding planes");
	Interface.frMandelboxColoring = gtk_frame_new("Mandelbox colouring parameters");

	//separators
	Interface.hSeparator1 = gtk_hseparator_new();
	Interface.vSeparator1 = gtk_vseparator_new();

	//buttons
	Interface.buRender = gtk_button_new_with_label("RENDER");
	Interface.buStop = gtk_button_new_with_label("STOP");
	Interface.buApplyBrighness = gtk_button_new_with_label("Apply changes");
	Interface.buSaveImage = gtk_button_new_with_label("Save JPG");
	Interface.buSavePNG = gtk_button_new_with_label("Save PNG");
	Interface.buSavePNG16 = gtk_button_new_with_label("Save PNG 16-bit");
	Interface.buSavePNG16Alpha = gtk_button_new_with_label("Save PNG 16-bit + Alpha");
	Interface.buFiles = gtk_button_new_with_label("Select file paths (output images, textures)");
	Interface.buColorGlow1 = gtk_color_button_new();
	Interface.buColorGlow2 = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorGlow1), "Glow color 1");
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorGlow2), "Glow color 2");
	Interface.buColorBackgroud1 = gtk_color_button_new();
	Interface.buColorBackgroud2 = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorBackgroud1), "Background color 1");
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorBackgroud2), "Background color 2");
	Interface.buLoadSettings = gtk_button_new_with_label("Load Settings");
	Interface.buSaveSettings = gtk_button_new_with_label("Save Settings");
	Interface.buUp = gtk_button_new();
	Interface.buDown = gtk_button_new();
	Interface.buLeft = gtk_button_new();
	Interface.buRight = gtk_button_new();
	Interface.buMoveUp = gtk_button_new();
	Interface.buMoveDown = gtk_button_new();
	Interface.buMoveLeft = gtk_button_new();
	Interface.buMoveRight = gtk_button_new();
	Interface.buForward = gtk_button_new_with_label("Forward");
	Interface.buBackward = gtk_button_new_with_label("backward");
	Interface.buInitNavigator = gtk_button_new_with_label("Init 3D Navigator");
	Interface.buAnimationRecordTrack = gtk_button_new_with_label("Record path");
	Interface.buAnimationContinueRecord = gtk_button_new_with_label("Continue recording");
	Interface.buAnimationRenderTrack = gtk_button_new_with_label("Render animation");
	Interface.buColorFog = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorFog), "Fog color");
	Interface.buColorSSAO = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorSSAO), "Screen space ambient occlusion color");
	Interface.buUpdateSSAO = gtk_button_new_with_label("Update image");
	Interface.buUpdateDOF = gtk_button_new_with_label("Update DOF");
	Interface.buColorAuxLightPre1 = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre1), "Colour of Auxiliary light #1");
	Interface.buColorAuxLightPre2 = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre2), "Colour of Auxiliary light #2");
	Interface.buColorAuxLightPre3 = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre3), "Colour of Auxiliary light #3");
	Interface.buColorAuxLightPre4 = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorAuxLightPre4), "Colour of Auxiliary light #4");
	Interface.buColorMainLight = gtk_color_button_new();
	gtk_color_button_set_title(GTK_COLOR_BUTTON(Interface.buColorMainLight), "Main light source colour");
	Interface.buDistributeLights = gtk_button_new_with_label("Distribute / update lights");
	Interface.buIFSNormalizeOffset = gtk_button_new_with_label("Normalize offset vector");
	Interface.buIFSNormalizeVectors = gtk_button_new_with_label("Normalize symetry vectors");
	Interface.buAnimationRecordKey = gtk_button_new_with_label("Record key-frame");
	Interface.buAnimationRenderFromKeys = gtk_button_new_with_label("Render from key-frames");
	Interface.buUndo = gtk_button_new_with_label("Undo");
	Interface.buRedo = gtk_button_new_with_label("Redo");
	Interface.buBuddhabrot = gtk_button_new_with_label("Render Buddhabrot");
	Interface.buRandomPalette = gtk_button_new_with_label("Random palette");
	Interface.buLoadSound = gtk_button_new_with_label("Load sound");
	Interface.buGetPaletteFromImage = gtk_button_new_with_label("Get palette from image");
	Interface.buTimeline = gtk_button_new_with_label("Timeline");

	//edit
	Interface.edit_va = gtk_entry_new();
	Interface.edit_vb = gtk_entry_new();
	Interface.edit_vc = gtk_entry_new();
	Interface.edit_julia_a = gtk_entry_new();
	Interface.edit_julia_b = gtk_entry_new();
	Interface.edit_julia_c = gtk_entry_new();
	Interface.edit_amin = gtk_entry_new();
	Interface.edit_amax = gtk_entry_new();
	Interface.edit_bmin = gtk_entry_new();
	Interface.edit_bmax = gtk_entry_new();
	Interface.edit_cmin = gtk_entry_new();
	Interface.edit_cmax = gtk_entry_new();
	Interface.edit_alfa = gtk_entry_new();
	Interface.edit_beta = gtk_entry_new();
	Interface.edit_gammaAngle = gtk_entry_new();
	Interface.edit_zoom = gtk_entry_new();
	Interface.edit_persp = gtk_entry_new();
	Interface.edit_maxN = gtk_entry_new();
	Interface.edit_minN = gtk_entry_new();
	Interface.edit_power = gtk_entry_new();
	Interface.edit_DE_thresh = gtk_entry_new();
	Interface.edit_DE_stepFactor = gtk_entry_new();
	Interface.edit_imageWidth = gtk_entry_new();
	Interface.edit_imageHeight = gtk_entry_new();
	Interface.edit_ambient = gtk_entry_new();
	Interface.edit_ambient_occlusion = gtk_entry_new();
	Interface.edit_brightness = gtk_entry_new();
	Interface.edit_gamma = gtk_entry_new();
	Interface.edit_glow = gtk_entry_new();
	Interface.edit_reflect = gtk_entry_new();
	Interface.edit_shading = gtk_entry_new();
	Interface.edit_shadows = gtk_entry_new();
	Interface.edit_specular = gtk_entry_new();
	Interface.edit_AmbientOcclusionQuality = gtk_entry_new();
	Interface.edit_step_forward = gtk_entry_new();
	Interface.edit_step_rotation = gtk_entry_new();
	Interface.edit_mouse_click_distance = gtk_entry_new();
	Interface.edit_animationDESpeed = gtk_entry_new();
	Interface.edit_color_seed = gtk_entry_new();
	Interface.edit_color_speed = gtk_entry_new();
	Interface.edit_tglad_folding_1 = gtk_entry_new();
	Interface.edit_tglad_folding_2 = gtk_entry_new();
	Interface.edit_spherical_folding_1 = gtk_entry_new();
	Interface.edit_spherical_folding_2 = gtk_entry_new();
	Interface.edit_mainLightIntensity = gtk_entry_new();
	Interface.edit_auxLightIntensity = gtk_entry_new();
	Interface.edit_auxLightNumber = gtk_entry_new();
	Interface.edit_auxLightMaxDist = gtk_entry_new();
	Interface.edit_auxLightRandomSeed = gtk_entry_new();
	Interface.edit_auxLightDistributionRadius = gtk_entry_new();
	Interface.edit_auxLightPre1x = gtk_entry_new();
	Interface.edit_auxLightPre1y = gtk_entry_new();
	Interface.edit_auxLightPre1z = gtk_entry_new();
	Interface.edit_auxLightPre1intensity = gtk_entry_new();
	Interface.edit_auxLightPre2x = gtk_entry_new();
	Interface.edit_auxLightPre2y = gtk_entry_new();
	Interface.edit_auxLightPre2z = gtk_entry_new();
	Interface.edit_auxLightPre2intensity = gtk_entry_new();
	Interface.edit_auxLightPre3x = gtk_entry_new();
	Interface.edit_auxLightPre3y = gtk_entry_new();
	Interface.edit_auxLightPre3z = gtk_entry_new();
	Interface.edit_auxLightPre3intensity = gtk_entry_new();
	Interface.edit_auxLightPre4x = gtk_entry_new();
	Interface.edit_auxLightPre4y = gtk_entry_new();
	Interface.edit_auxLightPre4z = gtk_entry_new();
	Interface.edit_auxLightPre4intensity = gtk_entry_new();
	Interface.edit_mainLightAlfa = gtk_entry_new();
	Interface.edit_mainLightBeta = gtk_entry_new();
	Interface.edit_auxLightVisibility = gtk_entry_new();
	Interface.edit_IFSScale = gtk_entry_new();
	Interface.edit_IFSAlfa = gtk_entry_new();
	Interface.edit_IFSBeta = gtk_entry_new();
	Interface.edit_IFSGamma = gtk_entry_new();
	Interface.edit_IFSOffsetX = gtk_entry_new();
	Interface.edit_IFSOffsetY = gtk_entry_new();
	Interface.edit_IFSOffsetZ = gtk_entry_new();
	Interface.edit_animationFramesPerKey = gtk_entry_new();
	Interface.edit_animationStartFrame = gtk_entry_new();
	Interface.edit_animationEndFrame = gtk_entry_new();
	Interface.edit_sound1FreqMin = gtk_entry_new();
	Interface.edit_sound1FreqMax = gtk_entry_new();
	Interface.edit_sound2FreqMin = gtk_entry_new();
	Interface.edit_sound2FreqMax = gtk_entry_new();
	Interface.edit_sound3FreqMin = gtk_entry_new();
	Interface.edit_sound3FreqMax = gtk_entry_new();
	Interface.edit_sound4FreqMin = gtk_entry_new();
	Interface.edit_sound4FreqMax = gtk_entry_new();
	Interface.edit_soundFPS = gtk_entry_new();
	Interface.edit_hybridIter1 = gtk_entry_new();
	Interface.edit_hybridIter2 = gtk_entry_new();
	Interface.edit_hybridIter3 = gtk_entry_new();
	Interface.edit_hybridIter4 = gtk_entry_new();
	Interface.edit_hybridIter5 = gtk_entry_new();
	Interface.edit_hybridPower1 = gtk_entry_new();
	Interface.edit_hybridPower2 = gtk_entry_new();
	Interface.edit_hybridPower3 = gtk_entry_new();
	Interface.edit_hybridPower4 = gtk_entry_new();
	Interface.edit_hybridPower5 = gtk_entry_new();
	Interface.edit_NavigatorAbsoluteDistance = gtk_entry_new();
	Interface.edit_stereoDistance = gtk_entry_new();
	Interface.edit_mandelboxScale = gtk_entry_new();
	Interface.edit_mandelboxFoldingLimit = gtk_entry_new();
	Interface.edit_mandelboxFoldingValue = gtk_entry_new();
	Interface.edit_mandelboxSpFoldingFixedRadius = gtk_entry_new();
	Interface.edit_mandelboxSpFoldingMinRadius = gtk_entry_new();
	Interface.edit_mandelboxRotationX1Alfa = gtk_entry_new();
	Interface.edit_mandelboxRotationX1Beta = gtk_entry_new();
	Interface.edit_mandelboxRotationX1Gamma = gtk_entry_new();
	Interface.edit_mandelboxRotationX2Alfa = gtk_entry_new();
	Interface.edit_mandelboxRotationX2Beta = gtk_entry_new();
	Interface.edit_mandelboxRotationX2Gamma = gtk_entry_new();
	Interface.edit_mandelboxRotationY1Alfa = gtk_entry_new();
	Interface.edit_mandelboxRotationY1Beta = gtk_entry_new();
	Interface.edit_mandelboxRotationY1Gamma = gtk_entry_new();
	Interface.edit_mandelboxRotationY2Alfa = gtk_entry_new();
	Interface.edit_mandelboxRotationY2Beta = gtk_entry_new();
	Interface.edit_mandelboxRotationY2Gamma = gtk_entry_new();
	Interface.edit_mandelboxRotationZ1Alfa = gtk_entry_new();
	Interface.edit_mandelboxRotationZ1Beta = gtk_entry_new();
	Interface.edit_mandelboxRotationZ1Gamma = gtk_entry_new();
	Interface.edit_mandelboxRotationZ2Alfa = gtk_entry_new();
	Interface.edit_mandelboxRotationZ2Beta = gtk_entry_new();
	Interface.edit_mandelboxRotationZ2Gamma = gtk_entry_new();
	Interface.edit_mandelboxRotationMainAlfa = gtk_entry_new();
	Interface.edit_mandelboxRotationMainBeta = gtk_entry_new();
	Interface.edit_mandelboxRotationMainGamma = gtk_entry_new();
	Interface.edit_mandelboxColorFactorR = gtk_entry_new();
	Interface.edit_mandelboxColorFactorSp1 = gtk_entry_new();
	Interface.edit_mandelboxColorFactorSp2 = gtk_entry_new();
	Interface.edit_mandelboxColorFactorX = gtk_entry_new();
	Interface.edit_mandelboxColorFactorY = gtk_entry_new();
	Interface.edit_mandelboxColorFactorZ = gtk_entry_new();

	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationX1Alfa), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationX1Beta), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationX1Gamma), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationY1Alfa), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationY1Beta), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationY1Gamma), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Alfa), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Beta), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationZ1Gamma), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationX2Alfa), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationX2Beta), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationX2Gamma), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationY2Alfa), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationY2Beta), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationY2Gamma), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Alfa), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Beta), 5);
	gtk_entry_set_width_chars(GTK_ENTRY(Interface.edit_mandelboxRotationZ2Gamma), 5);

	//combo
	//		fract type
	Interface.comboFractType = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Trigonometric");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Trigonometric with calculation of analytic DE");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Polynomic power 2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Polynomic power 2 - minus z");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Xenodreambuie's formula");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Hypercomplex");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Quaternion");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Menger sponge");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Tglad's formula (Mandelbox)");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Kaleidoscopic IFS");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Modified mandelbulb 1");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Modified mandelbulb 2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Modified mandelbulb 3");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboFractType), "Hybrid");
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboFractType), 1);
	//		image scale
	Interface.combo_imageScale = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1/10");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1/8");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1/6");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1/4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1/3");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1/2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "1");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "6");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "8");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.combo_imageScale), "Fit to window");
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.combo_imageScale), 11);
	//image format
	Interface.comboImageFormat = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboImageFormat), "JPEG");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboImageFormat), "PNG 8-bit");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboImageFormat), "PNG 16-bit");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboImageFormat), "PNG 16-bit with alpha channel");
	gtk_combo_box_set_active(GTK_COMBO_BOX(Interface.comboImageFormat), 0);

	Interface.comboHybridFormula1 = gtk_combo_box_new_text();
	Interface.comboHybridFormula2 = gtk_combo_box_new_text();
	Interface.comboHybridFormula3 = gtk_combo_box_new_text();
	Interface.comboHybridFormula4 = gtk_combo_box_new_text();
	Interface.comboHybridFormula5 = gtk_combo_box_new_text();
	AddComboTextsFractalFormula(GTK_COMBO_BOX(Interface.comboHybridFormula1));
	AddComboTextsFractalFormula(GTK_COMBO_BOX(Interface.comboHybridFormula2));
	AddComboTextsFractalFormula(GTK_COMBO_BOX(Interface.comboHybridFormula3));
	AddComboTextsFractalFormula(GTK_COMBO_BOX(Interface.comboHybridFormula4));
	AddComboTextsFractalFormula(GTK_COMBO_BOX(Interface.comboHybridFormula5));

	Interface.comboHybridDEMethod = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboHybridDEMethod), "Delta DE");
	gtk_combo_box_append_text(GTK_COMBO_BOX(Interface.comboHybridDEMethod), "Analytic for Mandelboxes and IFS");

	//progress bar
	Interface.progressBar = gtk_progress_bar_new();

	//checkboxes
	Interface.checkShadow = gtk_check_button_new_with_label("Shadows");
	Interface.checkAmbientOcclusion = gtk_check_button_new_with_label("Ambient occlusion");
	Interface.checkFastAmbientOcclusion = gtk_check_button_new_with_label("Ambient occlusion fast mode");
	Interface.checkIterThresh = gtk_check_button_new_with_label("Maxiter threshold mode");
	Interface.checkJulia = gtk_check_button_new_with_label("Julia mode");
	Interface.checkSlowShading = gtk_check_button_new_with_label("Not DE Shading mode (slow)");
	Interface.checkLimits = gtk_check_button_new_with_label("Enable limits");
	Interface.checkBitmapBackground = gtk_check_button_new_with_label("Textured background");
	Interface.checkColoring = gtk_check_button_new_with_label("Coloured surface");
	Interface.checkTgladMode = gtk_check_button_new_with_label("Tglad's folding mode      ");
	Interface.checkSphericalFoldingMode = gtk_check_button_new_with_label("Spherical folding mode      ");
	Interface.checkIFSFoldingMode = gtk_check_button_new_with_label("Kaleidoscopic IFS folding mode (parameters on Kaleidoscopic IFS tab)");
	Interface.checkFogEnabled = gtk_check_button_new_with_label("Enable fog         ");
	Interface.checkSSAOEnabled = gtk_check_button_new_with_label("Screen space ambient occlusion enable       ");
	Interface.checkDOFEnabled = gtk_check_button_new_with_label("Depth of field enable       ");
	Interface.checkZoomClickEnable = gtk_check_button_new_with_label("Enable zoom by mouse click");
	Interface.checkAuxLightPre1Enabled = gtk_check_button_new_with_label("Enable");
	Interface.checkAuxLightPre2Enabled = gtk_check_button_new_with_label("Enable");
	Interface.checkAuxLightPre3Enabled = gtk_check_button_new_with_label("Enable");
	Interface.checkAuxLightPre4Enabled = gtk_check_button_new_with_label("Enable");
	Interface.checkIFSAbsX = gtk_check_button_new_with_label("abs(x)");
	Interface.checkIFSAbsY = gtk_check_button_new_with_label("abs(y)");
	Interface.checkIFSAbsZ = gtk_check_button_new_with_label("abs(z)");
	Interface.checkAutoSaveImage = gtk_check_button_new_with_label("Auto-save");
	Interface.checkSoundEnabled = gtk_check_button_new_with_label("Enable animation by sound");
	Interface.checkHybridCyclic = gtk_check_button_new_with_label("Cyclic loop");
	Interface.checkNavigatorAbsoluteDistance = gtk_check_button_new_with_label("Absolute distance mode");
	Interface.checkNavigatorGoToSurface = gtk_check_button_new_with_label("Go close to indicated surface");
	Interface.checkFishEye = gtk_check_button_new_with_label("Fish eye");
	Interface.checkStraightRotation = gtk_check_button_new_with_label("Rotation without\nusing gamma\nangle");
	Interface.checkStereoEnabled = gtk_check_button_new_with_label("Enable stereoscopic rendering");
	Interface.checkMandelboxRotationsEnable = gtk_check_button_new_with_label("Enable rotation of each folding plane");

	//pixamps
	Interface.pixmap_up = gtk_image_new_from_file("icons/go-up.png");
	Interface.pixmap_down = gtk_image_new_from_file("icons/go-down.png");
	Interface.pixmap_left = gtk_image_new_from_file("icons/go-previous.png");
	Interface.pixmap_right = gtk_image_new_from_file("icons/go-next.png");
	Interface.pixmap_move_up = gtk_image_new_from_file("icons/go-up.png");
	Interface.pixmap_move_down = gtk_image_new_from_file("icons/go-down.png");
	Interface.pixmap_move_left = gtk_image_new_from_file("icons/go-previous.png");
	Interface.pixmap_move_right = gtk_image_new_from_file("icons/go-next.png");

	//labels
	Interface.label_animationFrame = gtk_label_new("Frame:");
	Interface.label_animationDistance = gtk_label_new("Estimated distance to fractal:");
	Interface.label_animationSpeed = gtk_label_new("Flight speed:");
	Interface.label_keyframeInfo = gtk_label_new("Frame: ,keyframe: ");
	Interface.label_fog_visibility = gtk_label_new("Visibility:");
	Interface.label_fog_visibility_front = gtk_label_new("Front:");
	Interface.label_SSAO_quality = gtk_label_new("Quality:");
	Interface.label_DOF_focus = gtk_label_new("Focus:");
	Interface.label_DOF_radius = gtk_label_new("Radius:");
	Interface.label_auxLightPre1 = gtk_label_new("Light #1:");
	Interface.label_auxLightPre2 = gtk_label_new("Light #2:");
	Interface.label_auxLightPre3 = gtk_label_new("Light #3:");
	Interface.label_auxLightPre4 = gtk_label_new("Light #4:");
	Interface.label_IFSx = gtk_label_new("symetry x");
	Interface.label_IFSy = gtk_label_new("symetry y");
	Interface.label_IFSz = gtk_label_new("symetry z");
	Interface.label_IFSalfa = gtk_label_new("alfa");
	Interface.label_IFSbeta = gtk_label_new("beta");
	Interface.label_IFSgamma = gtk_label_new("gamma");
	Interface.label_IFSdistance = gtk_label_new("distance");
	Interface.label_IFSintensity = gtk_label_new("intensity");
	Interface.label_IFSenabled = gtk_label_new("enabled");
	Interface.label_paletteOffset = gtk_label_new("offset:");
	Interface.label_soundEnvelope = gtk_label_new("sound envelope");
	gtk_misc_set_alignment(GTK_MISC(Interface.label_soundEnvelope), 0, 0);
	Interface.label_HybridFormula1 = gtk_label_new("Formula #1:");
	Interface.label_HybridFormula2 = gtk_label_new("Formula #2:");
	Interface.label_HybridFormula3 = gtk_label_new("Formula #3:");
	Interface.label_HybridFormula4 = gtk_label_new("Formula #4:");
	Interface.label_HybridFormula5 = gtk_label_new("Formula #5:");
	Interface.label_NavigatorEstimatedDistance = gtk_label_new("Estimated distance to the surface:");

	Interface.label_about = gtk_label_new("Mandelbulber 0.95\n"
		"author: Krzysztof Marczak\n"
		"Licence: GNU GPL\n"
		"www: http://sourceforge.net/projects/mandelbulber/");

	//sliders
	Interface.adjustmentFogDepth = gtk_adjustment_new(30.0, 0.1, 200.0, 0.1, 10.0, 0.1);
	Interface.sliderFogDepth = gtk_hscale_new(GTK_ADJUSTMENT(Interface.adjustmentFogDepth));
	Interface.adjustmentFogDepthFront = gtk_adjustment_new(20.0, 0.1, 200.0, 0.1, 10.0, 0.1);
	Interface.sliderFogDepthFront = gtk_hscale_new(GTK_ADJUSTMENT(Interface.adjustmentFogDepthFront));
	Interface.adjustmentSSAOQuality = gtk_adjustment_new(20, 1, 100, 1, 10, 1);
	Interface.sliderSSAOQuality = gtk_hscale_new(GTK_ADJUSTMENT(Interface.adjustmentSSAOQuality));
	Interface.adjustmentDOFFocus = gtk_adjustment_new(20.0, 0.1, 200.0, 0.1, 10.0, 0.1);
	Interface.sliderDOFFocus = gtk_hscale_new(GTK_ADJUSTMENT(Interface.adjustmentDOFFocus));
	Interface.adjustmentDOFRadius = gtk_adjustment_new(10.0, 0.1, 100.0, 0.1, 10.0, 0.1);
	Interface.sliderDOFRadius = gtk_hscale_new(GTK_ADJUSTMENT(Interface.adjustmentDOFRadius));
	Interface.adjustmentPaletteOffset = gtk_adjustment_new(0, 0, 256.0, 0.1, 10.0, 0.1);
	Interface.sliderPaletteOffset = gtk_hscale_new(GTK_ADJUSTMENT(Interface.adjustmentPaletteOffset));

	//colour palette
	dareaPalette = gtk_drawing_area_new();
	gtk_widget_set_size_request(dareaPalette, 640, 30);

	//sound
	Interface.dareaSound0 = gtk_drawing_area_new();
	gtk_widget_set_size_request(Interface.dareaSound0, 640, 40);
	Interface.dareaSoundA = gtk_drawing_area_new();
	gtk_widget_set_size_request(Interface.dareaSoundA, 640, 40);
	Interface.dareaSoundB = gtk_drawing_area_new();
	gtk_widget_set_size_request(Interface.dareaSoundB, 640, 40);
	Interface.dareaSoundC = gtk_drawing_area_new();
	gtk_widget_set_size_request(Interface.dareaSoundC, 640, 40);
	Interface.dareaSoundD = gtk_drawing_area_new();
	gtk_widget_set_size_request(Interface.dareaSoundD, 640, 40);

	//connected signals
	g_signal_connect(G_OBJECT(Interface.buRender), "clicked", G_CALLBACK(StartRendering), NULL);
	g_signal_connect(G_OBJECT(Interface.buStop), "clicked", G_CALLBACK(StopRendering), NULL);
	g_signal_connect(G_OBJECT(Interface.buApplyBrighness), "clicked", G_CALLBACK(PressedApplyBrigtness), NULL);
	g_signal_connect(G_OBJECT(Interface.buSaveImage), "clicked", G_CALLBACK(PressedSaveImage), NULL);
	g_signal_connect(G_OBJECT(Interface.buSavePNG), "clicked", G_CALLBACK(PressedSaveImagePNG), NULL);
	g_signal_connect(G_OBJECT(Interface.buSavePNG16), "clicked", G_CALLBACK(PressedSaveImagePNG16), NULL);
	g_signal_connect(G_OBJECT(Interface.buSavePNG16Alpha), "clicked", G_CALLBACK(PressedSaveImagePNG16Alpha), NULL);
	g_signal_connect(G_OBJECT(Interface.buLoadSettings), "clicked", G_CALLBACK(PressedLoadSettings), NULL);
	g_signal_connect(G_OBJECT(Interface.buSaveSettings), "clicked", G_CALLBACK(PressedSaveSettings), NULL);
	g_signal_connect(G_OBJECT(Interface.buFiles), "clicked", G_CALLBACK(CreateFilesDialog), NULL);
	g_signal_connect(G_OBJECT(Interface.buUp), "clicked", G_CALLBACK(PressedNavigatorUp), NULL);
	g_signal_connect(G_OBJECT(Interface.buDown), "clicked", G_CALLBACK(PressedNavigatorDown), NULL);
	g_signal_connect(G_OBJECT(Interface.buLeft), "clicked", G_CALLBACK(PressedNavigatorLeft), NULL);
	g_signal_connect(G_OBJECT(Interface.buRight), "clicked", G_CALLBACK(PressedNavigatorRight), NULL);
	g_signal_connect(G_OBJECT(Interface.buMoveUp), "clicked", G_CALLBACK(PressedNavigatorMoveUp), NULL);
	g_signal_connect(G_OBJECT(Interface.buMoveDown), "clicked", G_CALLBACK(PressedNavigatorMoveDown), NULL);
	g_signal_connect(G_OBJECT(Interface.buMoveLeft), "clicked", G_CALLBACK(PressedNavigatorMoveLeft), NULL);
	g_signal_connect(G_OBJECT(Interface.buMoveRight), "clicked", G_CALLBACK(PressedNavigatorMoveRight), NULL);
	g_signal_connect(G_OBJECT(Interface.buForward), "clicked", G_CALLBACK(PressedNavigatorForward), NULL);
	g_signal_connect(G_OBJECT(Interface.buBackward), "clicked", G_CALLBACK(PressedNavigatorBackward), NULL);
	g_signal_connect(G_OBJECT(Interface.buInitNavigator), "clicked", G_CALLBACK(PressedNavigatorInit), NULL);
	g_signal_connect(G_OBJECT(Interface.buAnimationRecordTrack), "clicked", G_CALLBACK(PressedAnimationRecord), NULL);
	g_signal_connect(G_OBJECT(Interface.buAnimationContinueRecord), "clicked", G_CALLBACK(PressedAnimationContinueRecording), NULL);
	g_signal_connect(G_OBJECT(Interface.buAnimationRenderTrack), "clicked", G_CALLBACK(PressedAnimationRender), NULL);
	g_signal_connect(G_OBJECT(Interface.adjustmentFogDepth), "value-changed", G_CALLBACK(ChangedSliderFog), NULL);
	g_signal_connect(G_OBJECT(Interface.adjustmentFogDepthFront), "value-changed", G_CALLBACK(ChangedSliderFog), NULL);
	g_signal_connect(G_OBJECT(Interface.checkFogEnabled), "clicked", G_CALLBACK(ChangedSliderFog), NULL);
	g_signal_connect(G_OBJECT(Interface.checkSSAOEnabled), "clicked", G_CALLBACK(PressedSSAOUpdate), NULL);
	g_signal_connect(G_OBJECT(Interface.buUpdateSSAO), "clicked", G_CALLBACK(PressedSSAOUpdate), NULL);
	g_signal_connect(G_OBJECT(Interface.buUpdateDOF), "clicked", G_CALLBACK(PressedDOFUpdate), NULL);
	g_signal_connect(G_OBJECT(Interface.buDistributeLights), "clicked", G_CALLBACK(PressedDistributeLights), NULL);
	g_signal_connect(G_OBJECT(Interface.buIFSNormalizeOffset), "clicked", G_CALLBACK(PressedIFSNormalizeOffset), NULL);
	g_signal_connect(G_OBJECT(Interface.buIFSNormalizeVectors), "clicked", G_CALLBACK(PressedIFSNormalizeVectors), NULL);
	g_signal_connect(G_OBJECT(Interface.buAnimationRecordKey), "clicked", G_CALLBACK(PressedRecordKeyframe), NULL);
	g_signal_connect(G_OBJECT(Interface.buAnimationRenderFromKeys), "clicked", G_CALLBACK(PressedKeyframeAnimationRender), NULL);
	g_signal_connect(G_OBJECT(Interface.buUndo), "clicked", G_CALLBACK(PressedUndo), NULL);
	g_signal_connect(G_OBJECT(Interface.buRedo), "clicked", G_CALLBACK(PressedRedo), NULL);
	g_signal_connect(G_OBJECT(Interface.buBuddhabrot), "clicked", G_CALLBACK(PressedBuddhabrot), NULL);
	g_signal_connect(G_OBJECT(Interface.adjustmentPaletteOffset), "value-changed", G_CALLBACK(ChangedSliderPaletteOffset), NULL);
	g_signal_connect(G_OBJECT(Interface.buRandomPalette), "clicked", G_CALLBACK(PressedRandomPalette), NULL);
	g_signal_connect(G_OBJECT(Interface.buLoadSound), "clicked", G_CALLBACK(PressedLoadSound), NULL);
	g_signal_connect(G_OBJECT(Interface.buGetPaletteFromImage), "clicked", G_CALLBACK(PressedGetPaletteFromImage), NULL);
	g_signal_connect(G_OBJECT(Interface.buTimeline), "clicked", G_CALLBACK(PressedTimeline), NULL);

	g_signal_connect(G_OBJECT(Interface.combo_imageScale), "changed", G_CALLBACK(ChangedComboScale), NULL);
	g_signal_connect(G_OBJECT(Interface.comboFractType), "changed", G_CALLBACK(ChangedComboFormula), NULL);
	g_signal_connect(G_OBJECT(Interface.checkTgladMode), "clicked", G_CALLBACK(ChangedTgladFoldingMode), NULL);
	g_signal_connect(G_OBJECT(Interface.checkJulia), "clicked", G_CALLBACK(ChangedJulia), NULL);
	g_signal_connect(G_OBJECT(Interface.checkSphericalFoldingMode), "clicked", G_CALLBACK(ChangedSphericalFoldingMode), NULL);
	g_signal_connect(G_OBJECT(Interface.checkIFSFoldingMode), "clicked", G_CALLBACK(ChangedIFSFoldingMode), NULL);
	g_signal_connect(G_OBJECT(Interface.checkLimits), "clicked", G_CALLBACK(ChangedLimits), NULL);
	g_signal_connect(G_OBJECT(Interface.checkMandelboxRotationsEnable), "clicked", G_CALLBACK(ChangedMandelboxRotations), NULL);

	gtk_signal_connect(GTK_OBJECT(dareaPalette), "expose-event", GTK_SIGNAL_FUNC(on_dareaPalette_expose), NULL);
	gtk_signal_connect(GTK_OBJECT(Interface.dareaSound0), "expose-event", GTK_SIGNAL_FUNC(on_dareaSound_expose), (void*) "0");
	gtk_signal_connect(GTK_OBJECT(Interface.dareaSoundA), "expose-event", GTK_SIGNAL_FUNC(on_dareaSound_expose), (void*) "A");
	gtk_signal_connect(GTK_OBJECT(Interface.dareaSoundB), "expose-event", GTK_SIGNAL_FUNC(on_dareaSound_expose), (void*) "B");
	gtk_signal_connect(GTK_OBJECT(Interface.dareaSoundC), "expose-event", GTK_SIGNAL_FUNC(on_dareaSound_expose), (void*) "C");
	gtk_signal_connect(GTK_OBJECT(Interface.dareaSoundD), "expose-event", GTK_SIGNAL_FUNC(on_dareaSound_expose), (void*) "D");

	//----------------------- main box -----------------------

	//	box buttons
	gtk_box_pack_start(GTK_BOX(Interface.boxMain), Interface.boxButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxButtons), Interface.buRender, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxButtons), Interface.buStop, true, true, 1);

	//	frame view point
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_view), Interface.frCoordinates, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frCoordinates), Interface.boxView);

	//		box coordinates
	gtk_box_pack_start(GTK_BOX(Interface.boxView), Interface.boxCoordinates, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxCoordinates), CreateEdit("0,0", "x:", 20, Interface.edit_va), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxCoordinates), CreateEdit("0,0", "y:", 20, Interface.edit_vb), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxCoordinates), CreateEdit("0,0", "z:", 20, Interface.edit_vc), false, false, 1);

	//		box angle
	gtk_box_pack_start(GTK_BOX(Interface.boxView), Interface.boxAngle, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAngle), CreateEdit("0,0", "alfa (yaw):", 15, Interface.edit_alfa), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAngle), CreateEdit("0,0", "beta (pitch):", 15, Interface.edit_beta), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAngle), CreateEdit("0,0", "gamma (roll):", 15, Interface.edit_gammaAngle), false, false, 1);

	//		box zoom
	gtk_box_pack_start(GTK_BOX(Interface.boxView), Interface.boxZoom, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxZoom), CreateEdit("2,5", "Close up (zoom):", 20, Interface.edit_zoom), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxZoom), CreateEdit("0,5", "perspective (FOV):", 5, Interface.edit_persp), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxZoom), Interface.checkFishEye, false, false, 1);

	//buttons arrows
	gtk_container_add(GTK_CONTAINER(Interface.buUp), Interface.pixmap_up);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows), Interface.buUp, 1, 2, 0, 1);
	gtk_container_add(GTK_CONTAINER(Interface.buDown), Interface.pixmap_down);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows), Interface.buDown, 1, 2, 2, 3);
	gtk_container_add(GTK_CONTAINER(Interface.buLeft), Interface.pixmap_left);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows), Interface.buLeft, 0, 1, 1, 2);
	gtk_container_add(GTK_CONTAINER(Interface.buRight), Interface.pixmap_right);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows), Interface.buRight, 2, 3, 1, 2);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_view), Interface.fr3Dnavigator, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.fr3Dnavigator), Interface.boxArrows);
	gtk_box_pack_start(GTK_BOX(Interface.boxArrows), Interface.boxArrows2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxArrows2), Interface.tableArrows, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxArrows2), Interface.checkStraightRotation, false, false, 1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkStraightRotation), true);

	//navigation
	gtk_box_pack_start(GTK_BOX(Interface.boxArrows), Interface.boxNavigation, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), Interface.buInitNavigator, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), Interface.boxNavigationButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigationButtons), Interface.buForward, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigationButtons), Interface.buBackward, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), CreateEdit(DoubleToString(0.5), "Step for camera moving multiplied by DE:", 5, Interface.edit_step_forward), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), Interface.checkNavigatorAbsoluteDistance, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), CreateEdit(DoubleToString(0.1), "Absolute movement distance:", 10, Interface.edit_NavigatorAbsoluteDistance), false, false,
			1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), CreateEdit(DoubleToString(10.0), "Rotation step in degrees", 5, Interface.edit_step_rotation), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), CreateEdit(DoubleToString(5.0), "Mouse click close-up ratio", 5, Interface.edit_mouse_click_distance), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), Interface.boxNavigationZooming, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigationZooming), Interface.checkZoomClickEnable, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigationZooming), Interface.checkNavigatorGoToSurface, false, false, 1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkZoomClickEnable), true);
	gtk_box_pack_start(GTK_BOX(Interface.boxNavigation), Interface.label_NavigatorEstimatedDistance, false, false, 1);

	//buttons arrows 2
	gtk_container_add(GTK_CONTAINER(Interface.buMoveUp), Interface.pixmap_move_up);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows2), Interface.buMoveUp, 1, 2, 0, 1);
	gtk_container_add(GTK_CONTAINER(Interface.buMoveDown), Interface.pixmap_move_down);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows2), Interface.buMoveDown, 1, 2, 2, 3);
	gtk_container_add(GTK_CONTAINER(Interface.buMoveLeft), Interface.pixmap_move_left);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows2), Interface.buMoveLeft, 0, 1, 1, 2);
	gtk_container_add(GTK_CONTAINER(Interface.buMoveRight), Interface.pixmap_move_right);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableArrows2), Interface.buMoveRight, 2, 3, 1, 2);

	gtk_box_pack_start(GTK_BOX(Interface.boxArrows), Interface.boxArrows3, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxArrows3), Interface.tableArrows2, false, false, 1);

	//	frame fractal
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_fractal), Interface.frFractal, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frFractal), Interface.boxFractal);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), CreateWidgetWithLabel("Fractal formula type:", Interface.comboFractType), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.checkJulia, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.boxJulia, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxJulia), CreateEdit("0,0", "Julia x:", 20, Interface.edit_julia_a), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxJulia), CreateEdit("0,0", "Julia y:", 20, Interface.edit_julia_b), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxJulia), CreateEdit("0,0", "Julia z:", 20, Interface.edit_julia_c), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.boxTgladFolding, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxTgladFolding), Interface.checkTgladMode, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxTgladFolding), CreateEdit("1,0", "Folding limit:", 5, Interface.edit_tglad_folding_1), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxTgladFolding), CreateEdit("2,0", "Folding value:", 5, Interface.edit_tglad_folding_2), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.boxSphericalFolding, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSphericalFolding), Interface.checkSphericalFoldingMode, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSphericalFolding), CreateEdit("1,0", "Fixed radius:", 5, Interface.edit_spherical_folding_1), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSphericalFolding), CreateEdit("0,5", "Min. radius:", 5, Interface.edit_spherical_folding_2), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.checkIFSFoldingMode, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.boxQuality, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxQuality), CreateEdit("8,0", "power:", 5, Interface.edit_power), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxQuality), CreateEdit("250", "maximum iter:", 5, Interface.edit_maxN), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxQuality), CreateEdit("1", "minimum iter:", 5, Interface.edit_minN), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxQuality), CreateEdit("1,0", "resolution:", 5, Interface.edit_DE_thresh), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxQuality), CreateEdit("1,0", "DE step factor:", 5, Interface.edit_DE_stepFactor), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxFractal), Interface.checkIterThresh, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_fractal), Interface.frLimits, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frLimits), Interface.boxLimits);
	gtk_box_pack_start(GTK_BOX(Interface.boxLimits), Interface.tableLimits, false, false, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableLimits), CreateEdit("-0,5", "x min:", 20, Interface.edit_amin), 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableLimits), CreateEdit("-0,5", "y min:", 20, Interface.edit_bmin), 1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableLimits), CreateEdit("-0,5", "z min:", 20, Interface.edit_cmin), 2, 3, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableLimits), CreateEdit("0,5", "x max:", 20, Interface.edit_amax), 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableLimits), CreateEdit("0,5", "y max:", 20, Interface.edit_bmax), 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableLimits), CreateEdit("0,5", "z max:", 20, Interface.edit_cmax), 2, 3, 1, 2);
	gtk_box_pack_start(GTK_BOX(Interface.boxLimits), Interface.checkLimits, false, false, 1);

	//frame image
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_image), Interface.frImage, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frImage), Interface.boxImage);

	gtk_box_pack_start(GTK_BOX(Interface.boxImage), Interface.boxImageRes, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxImageRes), CreateEdit("800", "image width:", 5, Interface.edit_imageWidth), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxImageRes), CreateEdit("600", "image height:", 5, Interface.edit_imageHeight), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxImageRes), CreateWidgetWithLabel("Scale in window:", Interface.combo_imageScale), false, false, 1);

	//frame Stereoscopic
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_image), Interface.frStereo, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frStereo), Interface.boxStereoscopic);

	gtk_box_pack_start(GTK_BOX(Interface.boxStereoscopic), Interface.boxStereoParams, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxStereoParams), CreateEdit("0,1", "Distance between eyes:", 20, Interface.edit_stereoDistance), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxStereoParams), Interface.checkStereoEnabled, false, false, 1);

	//frame Image saving
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_image), Interface.frImageSaving, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frImageSaving), Interface.boxImageSaving);

	gtk_box_pack_start(GTK_BOX(Interface.boxImageSaving), Interface.boxSaveImage, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSaveImage), Interface.buSaveImage, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSaveImage), Interface.buSavePNG, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSaveImage), Interface.buSavePNG16, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSaveImage), Interface.buSavePNG16Alpha, true, true, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxImageSaving), Interface.boxImageAutoSave, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxImageAutoSave), CreateWidgetWithLabel("Autosave / animation image format:", Interface.comboImageFormat), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxImageAutoSave), Interface.checkAutoSaveImage, false, false, 1);

	//frame effects
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_shaders), Interface.frEffects, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frEffects), Interface.boxEffects);

	gtk_box_pack_start(GTK_BOX(Interface.boxEffects), Interface.boxBrightness, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxBrightness), CreateEdit("1,0", "brightness:", 5, Interface.edit_brightness), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxBrightness), CreateEdit("1,0", "gamma:", 5, Interface.edit_gamma), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxEffects), Interface.boxShading, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading), CreateEdit("0,5", "shading:", 5, Interface.edit_shading), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading), CreateEdit("1,0", "direct light:", 5, Interface.edit_shadows), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading), CreateEdit("1,0", "specularity:", 5, Interface.edit_specular), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading), CreateEdit("0,0", "ambient:", 5, Interface.edit_ambient), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading), CreateEdit("2,0", "ambient occlusion:", 5, Interface.edit_ambient_occlusion), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxEffects), Interface.boxShading2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading2), CreateEdit("1,0", "glow:", 5, Interface.edit_glow), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading2), CreateEdit("0,0", "reflect:", 5, Interface.edit_reflect), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxShading2), CreateEdit("4", "ambient occlusion quality:", 5, Interface.edit_AmbientOcclusionQuality), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxEffects), Interface.boxEffectsChecks, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsChecks), Interface.checkShadow, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsChecks), Interface.checkAmbientOcclusion, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsChecks), Interface.checkFastAmbientOcclusion, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsChecks), Interface.checkSlowShading, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxEffects), Interface.boxEffectsChecks2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsChecks2), Interface.checkBitmapBackground, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_shaders), Interface.frPalette, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frPalette), Interface.boxPalette);

	gtk_box_pack_start(GTK_BOX(Interface.boxPalette), Interface.boxEffectsColoring, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsColoring), Interface.checkColoring, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsColoring), Interface.vSeparator1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsColoring), CreateEdit("123456", "Random seed:", 6, Interface.edit_color_seed), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsColoring), CreateEdit("1,0", "Colour speed:", 6, Interface.edit_color_speed), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsColoring), Interface.buRandomPalette, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxEffectsColoring), Interface.buGetPaletteFromImage, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxPalette), dareaPalette, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPalette), Interface.boxPaletteOffset, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPaletteOffset), Interface.label_paletteOffset, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPaletteOffset), Interface.sliderPaletteOffset, true, true, 1);

	//frame colors
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_shaders), Interface.frColors, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frColors), Interface.boxColors);

	gtk_box_pack_start(GTK_BOX(Interface.boxColors), Interface.boxGlowColor, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxGlowColor), CreateWidgetWithLabel("Glow color 1:", Interface.buColorGlow1), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxGlowColor), CreateWidgetWithLabel("Glow color 2:", Interface.buColorGlow2), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxGlowColor), CreateWidgetWithLabel("Background color 1:", Interface.buColorBackgroud1), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxGlowColor), CreateWidgetWithLabel("Background color 2:", Interface.buColorBackgroud2), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_shaders), Interface.buApplyBrighness, false, false, 1);

	//frame animation
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_animation), Interface.frAnimation, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frAnimation), Interface.boxAnimation);

	gtk_box_pack_start(GTK_BOX(Interface.boxAnimation), Interface.boxAnimationButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimationButtons), Interface.buAnimationRecordTrack, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimationButtons), Interface.buAnimationContinueRecord, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimationButtons), Interface.buAnimationRenderTrack, true, true, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxAnimation), Interface.boxAnimationEdits, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimationEdits), CreateEdit(DoubleToString(0.01), "Flight speed (DE multiplier):", 5, Interface.edit_animationDESpeed), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxAnimation), Interface.label_animationFrame, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimation), Interface.label_animationDistance, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimation), Interface.label_animationSpeed, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_animation), Interface.frKeyframeAnimation, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frKeyframeAnimation), Interface.boxKeyframeAnimation);

	gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimation), Interface.boxKeyframeAnimationButtons, false, false, 1);
	//gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimationButtons), Interface.buAnimationRecordKey, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimationButtons), Interface.buAnimationRenderFromKeys, false, false, 1);

	//gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimation), Interface.boxKeyframeAnimationButtons2, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimation), Interface.boxKeyframeAnimationEdits, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimationEdits), CreateEdit("100", "Frames per key:", 5, Interface.edit_animationFramesPerKey), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxKeyframeAnimation), Interface.label_keyframeInfo, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_animation), Interface.frAnimationFrames, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frAnimationFrames), Interface.boxAnimationEdits2);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimationEdits2), CreateEdit("0", "Start frame:", 5, Interface.edit_animationStartFrame), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxAnimationEdits2), CreateEdit("1000", "End frame:", 5, Interface.edit_animationEndFrame), false, false, 1);

	//---- tab pot effects
	//frame fog
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_posteffects), Interface.frPostFog, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frPostFog), Interface.boxPostFog);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostFog), Interface.boxFogButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFogButtons), Interface.checkFogEnabled, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFogButtons), CreateWidgetWithLabel("Fog color:", Interface.buColorFog), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostFog), Interface.boxFogSlider, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFogSlider), Interface.label_fog_visibility, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFogSlider), Interface.sliderFogDepth, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostFog), Interface.boxFogSlider2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFogSlider2), Interface.label_fog_visibility_front, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxFogSlider2), Interface.sliderFogDepthFront, true, true, 1);


	//frame SSAO
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_posteffects), Interface.frPostSSAO, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frPostSSAO), Interface.boxPostSSAO);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostSSAO), Interface.boxSSAOButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSSAOButtons), Interface.checkSSAOEnabled, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSSAOButtons), Interface.buUpdateSSAO, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostSSAO), Interface.boxSSAOSlider, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSSAOSlider), Interface.label_SSAO_quality, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSSAOSlider), Interface.sliderSSAOQuality, true, true, 1);

	//frame DOF
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_posteffects), Interface.frPostDOF, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frPostDOF), Interface.boxPostDOF);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostDOF), Interface.boxDOFButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxDOFButtons), Interface.checkDOFEnabled, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxDOFButtons), Interface.buUpdateDOF, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostDOF), Interface.boxDOFSlider1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxDOFSlider1), Interface.label_DOF_focus, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxDOFSlider1), Interface.sliderDOFFocus, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxPostDOF), Interface.boxDOFSlider2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxDOFSlider2), Interface.label_DOF_radius, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxDOFSlider2), Interface.sliderDOFRadius, true, true, 1);

	//gtk_box_pack_start(GTK_BOX(Interface.tab_box_posteffects), Interface.buBuddhabrot, true, true, 1);

	//---- tab Lights
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_lights), Interface.frMainLight, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frMainLight), Interface.boxMainLight);
	gtk_box_pack_start(GTK_BOX(Interface.boxMainLight), Interface.boxMainLightPosition, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMainLightPosition), CreateEdit("-45", "Horizontal angle relative to camera:", 6, Interface.edit_mainLightAlfa), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMainLightPosition), CreateEdit("45", "Vertical angle relative to camera:", 6, Interface.edit_mainLightBeta), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMainLightPosition), CreateWidgetWithLabel("Colour:", Interface.buColorMainLight), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_lights), Interface.frLightBallance, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frLightBallance), Interface.boxLightBallance);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightBallance), Interface.boxLightBrightness, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightBrightness), CreateEdit("1,0", "Main light intensity:", 6, Interface.edit_mainLightIntensity), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightBrightness), CreateEdit("1,0", "Auxiliary lights intensity:", 6, Interface.edit_auxLightIntensity), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightBrightness), CreateEdit("1,0", "Lights visibility:", 6, Interface.edit_auxLightVisibility), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_lights), Interface.frLightsParameters, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frLightsParameters), Interface.boxLightsParameters);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightsParameters), Interface.boxLightDistribution, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightDistribution), CreateEdit("0", "Number of aux. lights:", 6, Interface.edit_auxLightNumber), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightDistribution), CreateEdit("1234", "Random seed:", 6, Interface.edit_auxLightRandomSeed), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightDistribution), CreateEdit("0,1", "Maximum distance from fractal", 12, Interface.edit_auxLightMaxDist), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightsParameters), Interface.boxLightDistribution2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightDistribution2), CreateEdit("3.0", "Distribution radius of lights:", 6, Interface.edit_auxLightDistributionRadius), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightDistribution2), Interface.buDistributeLights, false, false, 1);

	//frame: predefined lights
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_lights), Interface.frPredefinedLights, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frPredefinedLights), Interface.boxPredefinedLights);

	gtk_box_pack_start(GTK_BOX(Interface.boxPredefinedLights), Interface.boxLightPre1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), Interface.label_auxLightPre1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), CreateEdit("3,0", "x:", 12, Interface.edit_auxLightPre1x), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), CreateEdit("-3,0", "y:", 12, Interface.edit_auxLightPre1y), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), CreateEdit("-3,0", "z:", 12, Interface.edit_auxLightPre1z), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), CreateEdit("1,0", "intensity:", 6, Interface.edit_auxLightPre1intensity), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), Interface.buColorAuxLightPre1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre1), Interface.checkAuxLightPre1Enabled, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxPredefinedLights), Interface.boxLightPre2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), Interface.label_auxLightPre2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), CreateEdit("-3,0", "x:", 12, Interface.edit_auxLightPre2x), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), CreateEdit("-3,0", "y:", 12, Interface.edit_auxLightPre2y), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), CreateEdit("0,0", "z:", 12, Interface.edit_auxLightPre2z), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), CreateEdit("1,0", "intensity:", 6, Interface.edit_auxLightPre2intensity), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), Interface.buColorAuxLightPre2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre2), Interface.checkAuxLightPre2Enabled, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxPredefinedLights), Interface.boxLightPre3, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), Interface.label_auxLightPre3, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), CreateEdit("1,0", "x:", 12, Interface.edit_auxLightPre3x), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), CreateEdit("3,0", "y:", 12, Interface.edit_auxLightPre3y), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), CreateEdit("-1,0", "z:", 12, Interface.edit_auxLightPre3z), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), CreateEdit("1,0", "intensity:", 6, Interface.edit_auxLightPre3intensity), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), Interface.buColorAuxLightPre3, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre3), Interface.checkAuxLightPre3Enabled, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxPredefinedLights), Interface.boxLightPre4, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), Interface.label_auxLightPre4, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), CreateEdit("1,0", "x:", 12, Interface.edit_auxLightPre4x), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), CreateEdit("-1,0", "y:", 12, Interface.edit_auxLightPre4y), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), CreateEdit("-3,0", "z:", 12, Interface.edit_auxLightPre4z), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), CreateEdit("1,0", "intensity:", 6, Interface.edit_auxLightPre4intensity), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), Interface.buColorAuxLightPre4, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLightPre4), Interface.checkAuxLightPre4Enabled, false, false, 1);

	//tab IFS
	//frame: main IFS
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_IFS), Interface.frIFSMain, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frIFSMain), Interface.boxIFSMain);

	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMain), Interface.boxIFSMainEdit, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit), CreateEdit("2,0", "scale:", 6, Interface.edit_IFSScale), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit), CreateEdit("0", "rotation alfa:", 6, Interface.edit_IFSAlfa), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit), CreateEdit("0", "rotation beta:", 6, Interface.edit_IFSBeta), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit), CreateEdit("0", "rotation gamma:", 6, Interface.edit_IFSGamma), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMain), Interface.boxIFSMainEdit2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit2), CreateEdit("1", "offset x:", 6, Interface.edit_IFSOffsetX), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit2), CreateEdit("0", "offset y:", 6, Interface.edit_IFSOffsetY), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit2), CreateEdit("0", "offset z:", 6, Interface.edit_IFSOffsetZ), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit2), Interface.checkIFSAbsX, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit2), Interface.checkIFSAbsY, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSMainEdit2), Interface.checkIFSAbsZ, false, false, 1);

	//frame: IFS params
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_IFS), Interface.frIFSParams, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frIFSParams), Interface.boxIFSParams);

	gtk_box_pack_start(GTK_BOX(Interface.boxIFSParams), Interface.tableIFSParams, false, false, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSx, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSy, 1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSz, 2, 3, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSalfa, 3, 4, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSbeta, 4, 5, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSgamma, 5, 6, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSdistance, 6, 7, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSintensity, 7, 8, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.label_IFSenabled, 8, 9, 0, 1);

	for (int i = 0; i < IFS_number_of_vectors; i++)
	{
		Interface.IFSParams[i].editIFSx = gtk_entry_new();
		Interface.IFSParams[i].editIFSy = gtk_entry_new();
		Interface.IFSParams[i].editIFSz = gtk_entry_new();
		Interface.IFSParams[i].editIFSalfa = gtk_entry_new();
		Interface.IFSParams[i].editIFSbeta = gtk_entry_new();
		Interface.IFSParams[i].editIFSgamma = gtk_entry_new();
		Interface.IFSParams[i].editIFSdistance = gtk_entry_new();
		Interface.IFSParams[i].editIFSintensity = gtk_entry_new();
		Interface.IFSParams[i].checkIFSenabled = gtk_check_button_new();
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSx), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSy), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSz), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSalfa), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSbeta), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSgamma), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSdistance), 6);
		gtk_entry_set_width_chars(GTK_ENTRY(Interface.IFSParams[i].editIFSintensity), 6);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSx, 0, 1, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSy, 1, 2, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSz, 2, 3, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSalfa, 3, 4, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSbeta, 4, 5, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSgamma, 5, 6, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSdistance, 6, 7, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].editIFSintensity, 7, 8, i + 1, i + 2);
		gtk_table_attach_defaults(GTK_TABLE(Interface.tableIFSParams), Interface.IFSParams[i].checkIFSenabled, 8, 9, i + 1, i + 2);
	}

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_IFS), Interface.boxIFSButtons, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSButtons), Interface.buIFSNormalizeOffset, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxIFSButtons), Interface.buIFSNormalizeVectors, false, false, 1);

	//tab hybrid formula
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_hybrid), Interface.frHybrid, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frHybrid), Interface.boxHybrid);

	gtk_box_pack_start(GTK_BOX(Interface.boxHybrid), Interface.tableHybridParams, false, false, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.label_HybridFormula1, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.label_HybridFormula2, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.label_HybridFormula3, 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.label_HybridFormula4, 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.label_HybridFormula5, 0, 1, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.comboHybridFormula1, 1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.comboHybridFormula2, 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.comboHybridFormula3, 1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.comboHybridFormula4, 1, 2, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), Interface.comboHybridFormula5, 1, 2, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("3", "  iterations:", 6, Interface.edit_hybridIter1), 2, 3, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("3", "  iterations:", 6, Interface.edit_hybridIter2), 2, 3, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("3", "  iterations:", 6, Interface.edit_hybridIter3), 2, 3, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("3", "  iterations:", 6, Interface.edit_hybridIter4), 2, 3, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("3", "  iterations:", 6, Interface.edit_hybridIter5), 2, 3, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("2", "power/scale:", 6, Interface.edit_hybridPower1), 3, 4, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("2", "power/scale:", 6, Interface.edit_hybridPower2), 3, 4, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("2", "power/scale:", 6, Interface.edit_hybridPower3), 3, 4, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("2", "power/scale:", 6, Interface.edit_hybridPower4), 3, 4, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableHybridParams), CreateEdit("2", "power/scale:", 6, Interface.edit_hybridPower5), 3, 4, 4, 5);

	gtk_box_pack_start(GTK_BOX(Interface.boxHybrid), Interface.checkHybridCyclic, false, false, 1);

	//tab Mandelbox
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_mandelbox), Interface.frMandelboxMainParams, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frMandelboxMainParams), Interface.boxMandelboxMainParams);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams), Interface.boxMandelboxMainParams1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams1), CreateEdit("2", "Scale", 6, Interface.edit_mandelboxScale), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams), Interface.boxMandelboxMainParams2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams2), CreateEdit("1", "Folding limit", 6, Interface.edit_mandelboxFoldingLimit), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams2), CreateEdit("2", "Folding value", 6, Interface.edit_mandelboxFoldingValue), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams2), CreateEdit("1", "Fixed radius", 6, Interface.edit_mandelboxSpFoldingFixedRadius), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxMainParams2), CreateEdit("0,5", "Min radius", 6, Interface.edit_mandelboxSpFoldingMinRadius), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_mandelbox), Interface.frMandelboxRotations, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frMandelboxRotations), Interface.boxMandelboxRotations);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxRotations), Interface.boxMandelboxRotationMain, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxRotations), Interface.checkMandelboxRotationsEnable, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxRotationMain), CreateEdit("0", "Main rotation: alfa", 6, Interface.edit_mandelboxRotationMainAlfa), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxRotationMain), CreateEdit("0", "beta", 6, Interface.edit_mandelboxRotationMainBeta), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxRotationMain), CreateEdit("0", "gamma", 6, Interface.edit_mandelboxRotationMainGamma), false, false, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Negative plane"), 1, 4, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Positive plane"), 4, 7, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Alfa"), 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Beta"), 2, 3, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Gamma"), 3, 4, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Alfa"), 4, 5, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Beta"), 5, 6, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Gamma"), 6, 7, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("X Axis"), 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationX1Alfa, 1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationX1Beta, 2, 3, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationX1Gamma, 3, 4, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationX2Alfa, 4, 5, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationX2Beta, 5, 6, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationX2Gamma, 6, 7, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Y Axis"), 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationY1Alfa, 1, 2, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationY1Beta, 2, 3, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationY1Gamma, 3, 4, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationY2Alfa, 4, 5, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationY2Beta, 5, 6, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationY2Gamma, 6, 7, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), gtk_label_new("Z Axis"), 0, 1, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationZ1Alfa, 1, 2, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationZ1Beta, 2, 3, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationZ1Gamma, 3, 4, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationZ2Alfa, 4, 5, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationZ2Beta, 5, 6, 4, 5);
	gtk_table_attach_defaults(GTK_TABLE(Interface.tableMandelboxRotations), Interface.edit_mandelboxRotationZ2Gamma, 6, 7, 4, 5);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxRotations), Interface.tableMandelboxRotations, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.tab_box_mandelbox), Interface.frMandelboxColoring, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frMandelboxColoring), Interface.boxMandelboxColoring);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColoring), Interface.boxMandelboxColor1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColor1), CreateEdit("0", "Resultant absolute value component", 6, Interface.edit_mandelboxColorFactorR), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColoring), Interface.boxMandelboxColor2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColor2), CreateEdit("0,1", "X plane component", 6, Interface.edit_mandelboxColorFactorX), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColor2), CreateEdit("0,2", "Y plane component", 6, Interface.edit_mandelboxColorFactorY), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColor2), CreateEdit("0,3", "Z plane component", 6, Interface.edit_mandelboxColorFactorZ), false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColoring), Interface.boxMandelboxColor3, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColor3), CreateEdit("5,0", "Min radius component", 6, Interface.edit_mandelboxColorFactorSp1), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxMandelboxColor3), CreateEdit("1,0", "Fixed radius component", 6, Interface.edit_mandelboxColorFactorSp2), false, false, 1);

	//tab sound
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_sound), Interface.frSound, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frSound), Interface.boxSound);

	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.boxSoundMisc, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundMisc), Interface.buLoadSound, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundMisc), CreateEdit("25", "Animation FPS", 6, Interface.edit_soundFPS), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundMisc), Interface.checkSoundEnabled, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.label_soundEnvelope, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.dareaSound0, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.boxSoundBand1, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand1), CreateEdit("1", "Band 1: min freq [Hz]", 6, Interface.edit_sound1FreqMin), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand1), CreateEdit("100", "max freq [Hz]", 6, Interface.edit_sound1FreqMax), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.dareaSoundA, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.boxSoundBand2, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand2), CreateEdit("100", "Band 2: min freq [Hz]", 6, Interface.edit_sound2FreqMin), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand2), CreateEdit("500", "max freq [Hz]", 6, Interface.edit_sound2FreqMax), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.dareaSoundB, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.boxSoundBand3, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand3), CreateEdit("500", "Band 3: min freq [Hz]", 6, Interface.edit_sound3FreqMin), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand3), CreateEdit("2000", "max freq [Hz]", 6, Interface.edit_sound3FreqMax), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.dareaSoundC, false, false, 1);

	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.boxSoundBand4, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand4), CreateEdit("2000", "Band 4: min freq [Hz]", 6, Interface.edit_sound4FreqMin), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSoundBand4), CreateEdit("20000", "max freq [Hz]", 6, Interface.edit_sound4FreqMax), false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxSound), Interface.dareaSoundD, false, false, 1);

	//tab About...
	gtk_box_pack_start(GTK_BOX(Interface.tab_box_about), Interface.label_about, false, false, 1);

	//tabs
	gtk_box_pack_start(GTK_BOX(Interface.boxMain), Interface.tabs, false, false, 1);

	//setup files button
	gtk_box_pack_start(GTK_BOX(Interface.boxMain), Interface.buFiles, false, false, 1);

	//Load / Save settings
	gtk_box_pack_start(GTK_BOX(Interface.boxMain), Interface.frLoadSave, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frLoadSave), Interface.boxLoadSave);

	gtk_box_pack_start(GTK_BOX(Interface.boxLoadSave), Interface.buLoadSettings, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLoadSave), Interface.buSaveSettings, true, true, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLoadSave), Interface.buUndo, false, false, 1);
	gtk_box_pack_start(GTK_BOX(Interface.boxLoadSave), Interface.buRedo, false, false, 1);

	//Keyframe aniation
	gtk_box_pack_start(GTK_BOX(Interface.boxMain), Interface.frKeyframeAnimation2, false, false, 1);
	gtk_container_add(GTK_CONTAINER(Interface.frKeyframeAnimation2), Interface.boxBottomKeyframeAnimation);
	gtk_box_pack_start(GTK_BOX(Interface.boxBottomKeyframeAnimation), Interface.buTimeline, true, true, 1);

	//progressbar
	gtk_box_pack_start(GTK_BOX(Interface.boxMain), Interface.progressBar, false, false, 1);

	//tabs
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_fractal, Interface.tab_label_fractal);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_IFS, Interface.tab_label_IFS);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_hybrid, Interface.tab_label_hybrid);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_mandelbox, Interface.tab_label_mandelbox);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_view, Interface.tab_label_view);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_shaders, Interface.tab_label_shaders);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_lights, Interface.tab_label_lights);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_image, Interface.tab_label_image);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_posteffects, Interface.tab_label_posteffects);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_animation, Interface.tab_label_animation);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_sound, Interface.tab_label_sound);
	gtk_notebook_append_page(GTK_NOTEBOOK(Interface.tabs), Interface.tab_box_about, Interface.tab_label_about);

	//main window pack
	gtk_container_add(GTK_CONTAINER(window_interface), Interface.boxMain);

	CreateTooltips();

	gtk_widget_show_all(window_interface);

	ChangedComboFormula(NULL, NULL);
	ChangedTgladFoldingMode(NULL, NULL);
	ChangedJulia(NULL, NULL);
	ChangedSphericalFoldingMode(NULL, NULL);
	ChangedLimits(NULL, NULL);
	ChangedMandelboxRotations(NULL, NULL);

	//Writing default settings
	WriteInterface(default_settings);
	interfaceCreated = true;
	renderRequest = false;

  g_timeout_add (100,(GSourceFunc)CallerTimerLoop,NULL);

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
}

void CreateTooltips(void)
{
	gtk_widget_set_tooltip_text(Interface.boxCoordinates, "Coordinates of camera view point in fractal units");
	gtk_widget_set_tooltip_text(Interface.comboFractType, "Type of fractal formula");
	gtk_widget_set_tooltip_text(Interface.checkJulia, "Fractal will be rendered as Julia fractal\naccording to coordinates of Julia constant");
	gtk_widget_set_tooltip_text(Interface.boxJulia, "Coordinates of camera view point in fractal units");
	gtk_widget_set_tooltip_text(Interface.boxCoordinates, "Coordinates of camera view point in fractal units");
	gtk_widget_set_tooltip_text(Interface.edit_power, "Fractal formula power\nOnly works with trigonometric formulas\nWhen using Tglad's formula, it is a scale factor");
	gtk_widget_set_tooltip_text(Interface.edit_maxN, "Maximum number of iterations");
	gtk_widget_set_tooltip_text(Interface.edit_minN, "Minimum number of iterations");
	gtk_widget_set_tooltip_text(Interface.edit_DE_thresh, "Dynamic DE threshold factor.\n1 = DE threhold equals to 1 screen pixel\nHigher value gives more details");
	gtk_widget_set_tooltip_text(Interface.edit_DE_stepFactor, "DE steps fractor.\n1 -> step = Estimated Distance\nHigher value gives higher accuracy of finding fractal surface");
	gtk_widget_set_tooltip_text(Interface.checkIterThresh, "Iteration count threshold mode");
	gtk_widget_set_tooltip_text(Interface.checkLimits, "Enables cross-sections of fractal");
	gtk_widget_set_tooltip_text(Interface.boxLimits, "Coordinates of cross-sections (limit box)");
	gtk_widget_set_tooltip_text(Interface.edit_alfa, "Camera rotation around vertical axis");
	gtk_widget_set_tooltip_text(Interface.edit_beta, "Camera rotation around horizontal axis");
	gtk_widget_set_tooltip_text(Interface.edit_zoom, "Camera size. Smaller value gives higher zoom (distance between camera and viewpoint)");
	gtk_widget_set_tooltip_text(Interface.edit_persp, "Perspective ratio\n0 = Axonometric projection\nHigher value gives deeper perspective effect");
	gtk_widget_set_tooltip_text(Interface.tableArrows, "Camera rotation");
	gtk_widget_set_tooltip_text(Interface.tableArrows2, "Camera movement");
	gtk_widget_set_tooltip_text(Interface.buInitNavigator, "Initialise camera.\nCamera viewpoint will be moved far and camera very close to viewpoint");
	gtk_widget_set_tooltip_text(Interface.buForward, "Move camera forward");
	gtk_widget_set_tooltip_text(Interface.buBackward, "Move camera backward");
	gtk_widget_set_tooltip_text(Interface.edit_step_forward, "Step length factor\nstep = factor * estimated_distance_to_fractal");
	gtk_widget_set_tooltip_text(Interface.edit_step_rotation, "Rotation step in degrees");
	gtk_widget_set_tooltip_text(Interface.edit_imageWidth, "Final image width");
	gtk_widget_set_tooltip_text(Interface.edit_imageHeight, "Final image height");
	gtk_widget_set_tooltip_text(Interface.combo_imageScale, "Image scale in render window\nSmaller value <-> smaller render window");
	gtk_widget_set_tooltip_text(Interface.edit_brightness, "Image brightness");
	gtk_widget_set_tooltip_text(Interface.edit_gamma, "Image gamma");
	gtk_widget_set_tooltip_text(Interface.buApplyBrighness, "Applying all shaders changes during rendering and after rendering");
	gtk_widget_set_tooltip_text(Interface.edit_shading, "angle of incidence of light effect intensity");
	gtk_widget_set_tooltip_text(Interface.edit_shadows, "shadows intensity");
	gtk_widget_set_tooltip_text(Interface.edit_specular, "intensity of specularity effect");
	gtk_widget_set_tooltip_text(Interface.edit_ambient, "intensity of global ambient light");
	gtk_widget_set_tooltip_text(Interface.edit_ambient_occlusion, "intensity of ambient occlusion effect");
	gtk_widget_set_tooltip_text(Interface.edit_glow, "intensity of glow effect");
	gtk_widget_set_tooltip_text(Interface.edit_reflect, "intensity of texture reflection (environment mapping effect)");
	gtk_widget_set_tooltip_text(Interface.edit_AmbientOcclusionQuality,
			"ambient occlusion quality\n1 -> 8 rays\n3 -> 64 rays\n5 -> 165 rays\n10 -> 645 rays\n30 -> 5702 rays (hardcore!!!)");
	gtk_widget_set_tooltip_text(Interface.checkShadow, "Enable shadow");
	gtk_widget_set_tooltip_text(Interface.checkAmbientOcclusion, "Enable ambient occlusion effect");
	gtk_widget_set_tooltip_text(Interface.checkFastAmbientOcclusion,
			"Switch to ambient occlusion based on orbit traps. Very fast but works properly only with trigonometric Mandelbulbs");
	gtk_widget_set_tooltip_text(Interface.checkSlowShading,
			"Enable calculation of light's angle of incidence based on fake gradient estimation\nVery slow but works with all fractal formulas");
	gtk_widget_set_tooltip_text(Interface.checkBitmapBackground, "Enable spherical wrapped textured background");
	gtk_widget_set_tooltip_text(Interface.checkColoring, "Enable fractal coloring algorithm");
	gtk_widget_set_tooltip_text(Interface.edit_color_seed, "Seed for random fractal colors");
	gtk_widget_set_tooltip_text(Interface.buColorGlow1, "Glow color - low intensity area");
	gtk_widget_set_tooltip_text(Interface.buColorGlow2, "Glow color - high intesity area");
	gtk_widget_set_tooltip_text(Interface.buColorBackgroud1, "Color of background top");
	gtk_widget_set_tooltip_text(Interface.buColorBackgroud2, "Color of background bottom");
	gtk_widget_set_tooltip_text(
			Interface.buAnimationRecordTrack,
			"Record of flight path. First you should set camera starting position using 3D Navigator tool and setup low resolution of image. For change flight direction please use mouse pointer");
	gtk_widget_set_tooltip_text(Interface.buAnimationRenderTrack, "Render animation according to recorded flight path");
	gtk_widget_set_tooltip_text(Interface.edit_animationDESpeed, "Flight speed - depends on estimated distance to fractal surface");
	gtk_widget_set_tooltip_text(Interface.buRender, "Render and save image");
	gtk_widget_set_tooltip_text(Interface.buStop, "Terminate rendering");
	gtk_widget_set_tooltip_text(Interface.buFiles, "Configure file paths");
	gtk_widget_set_tooltip_text(Interface.buLoadSettings, "Load fractal settings");
	gtk_widget_set_tooltip_text(Interface.buSaveSettings, "Save fractal settings");
	gtk_widget_set_tooltip_text(Interface.buColorBackgroud1, "Color of background top");
	gtk_widget_set_tooltip_text(Interface.edit_mouse_click_distance, "Close-up ratio in mouse clicking mode");
	gtk_widget_set_tooltip_text(Interface.frKeyframeAnimation, "Function for rendering animation with keyframes.\n"
		"All keyframes are stored in keyframes/keyframeXXXXX.fract files\n"
		"Keyframe files are settings files and can be also loaded and saved using Load Settings and Save Settings\n"
		"Most of floating point parameters are interpolated using Catmull-Rom Splines");
	gtk_widget_set_tooltip_text(Interface.frSound, "Sound file can be used for control of some fractal or shaders parameters.\n"
		"After loading sound file, the sound is divided into 5 channels:\n"
		"s0 - sound envelope (absolute value of sound amplitude)\n"
		"sa, sb, sc, sd - selected frequncy bands of sound\n"
		"To animate selected parameter you have to put instead of parameter value, following string:\n"
		"sx amp - where sx is channel name, amp is a gain for channel\n"
		"for example, in IFS parameters you can put: rotation alfa = s0 1.0, rotation beta = sc -0.3\n"
		"Animation must be rendered in key-frame mode");
	gtk_widget_set_tooltip_text(Interface.checkTgladMode, "Additional formula modificator. Cubic folding from Mandelbox formula");
	gtk_widget_set_tooltip_text(Interface.checkSphericalFoldingMode, "Additional formula modificator. Spherical folding from Mandelbox formula");
	gtk_widget_set_tooltip_text(Interface.comboHybridFormula1, "1st formula in sequence");
	gtk_widget_set_tooltip_text(Interface.comboHybridFormula2, "2nd formula in sequence");
	gtk_widget_set_tooltip_text(Interface.comboHybridFormula3, "3rd formula in sequence");
	gtk_widget_set_tooltip_text(Interface.comboHybridFormula4, "4th formula in sequence");
	gtk_widget_set_tooltip_text(Interface.comboHybridFormula5,
			"5th formula in sequence. When cyclic loop is disable, then this formula will be used for the rest of iterations and couldn't be set as NONE");
	gtk_widget_set_tooltip_text(Interface.edit_hybridIter1, "Number of iterations for the 1st formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridIter2, "Number of iterations for the 2nd formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridIter3, "Number of iterations for the 3rd formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridIter4, "Number of iterations for the 4th formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridIter5, "Number of iterations for the 5th formula (not used when cyclic loop is enabled)");
	gtk_widget_set_tooltip_text(Interface.edit_hybridPower1, "Power / scale parameter for the 1st formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridPower2, "Power / scale parameter for the 2nd formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridPower3, "Power / scale parameter for the 3rd formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridPower4, "Power / scale parameter for the 4th formula");
	gtk_widget_set_tooltip_text(Interface.edit_hybridPower5, "Power / scale parameter for the 5th formula");
	gtk_widget_set_tooltip_text(Interface.checkHybridCyclic, "When enable, formula sequence is looped");
	gtk_widget_set_tooltip_text(Interface.checkNavigatorAbsoluteDistance, "Movement step distance will be constant (absolute)");
	gtk_widget_set_tooltip_text(Interface.edit_NavigatorAbsoluteDistance, "Absolute movement step distance");
	gtk_widget_set_tooltip_text(Interface.edit_color_speed, "Colour variation speed");
	gtk_widget_set_tooltip_text(Interface.buRandomPalette, "Generates random palete according to entered random seed");
	gtk_widget_set_tooltip_text(Interface.buGetPaletteFromImage, "Grab colours from selected image");
	gtk_widget_set_tooltip_text(Interface.sliderFogDepth, "Visibility distance measured from Front value");
	gtk_widget_set_tooltip_text(Interface.sliderFogDepth, "Front distance of fog");

}

bool ReadComandlineParams(int argc, char *argv[])
{
	noGUIdata.lowMemMode = false;
	noGUIdata.animMode = false;
	noGUIdata.keyframeMode = false;
	noGUIdata.playMode = false;
	noGUIdata.startFrame = 0;
	noGUIdata.endFrame = 99999;
	noGUIdata.imageFormat = imgFormatJPG;
	strcpy(noGUIdata.settingsFile, "default.fract");
	bool result = true;

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			//printf("%s*\n", argv[i]);
			if (strcmp(argv[i], "-nogui") == 0)
			{
				noGUI = true;
				printf("commandline: no GUI mode\n");
				continue;
			}
			if (strcmp(argv[i], "-lowmem") == 0)
			{
				noGUIdata.lowMemMode = true;
				printf("commandline: Low memory mode\n");
				continue;
			}
			if (strcmp(argv[i], "-flight") == 0)
			{
				noGUIdata.animMode = true;
				noGUIdata.playMode = true;
				printf("commandline: flight mode\n");
				continue;
			}
			if (strcmp(argv[i], "-keyframe") == 0)
			{
				noGUIdata.animMode = true;
				noGUIdata.keyframeMode = true;
				printf("commandline: keyframe mode\n");
				continue;
			}
			if (strcmp(argv[i], "-start") == 0)
			{
				i++;
				if (i < argc)
				{
					noGUIdata.startFrame = atoi(argv[i]);
					printf("commandline: start frame = %d\n", noGUIdata.startFrame);
					continue;
				}
			}
			if (strcmp(argv[i], "-end") == 0)
			{
				i++;
				if (i < argc)
				{
					noGUIdata.endFrame = atoi(argv[i]);
					printf("commandline: end frame = %d\n", noGUIdata.endFrame);
					continue;
				}
			}
			if (strcmp(argv[i], "-format") == 0)
			{
				i++;
				if (i < argc && (strcmp(argv[i], "jpg") == 0))
				{
					noGUIdata.imageFormat = imgFormatJPG;
					printf("commandline: JPG image format\n");
					continue;
				}
				else if (i < argc && (strcmp(argv[i], "png") == 0))
				{
					noGUIdata.imageFormat = imgFormatPNG;
					printf("commandline: PNG image format\n");
					continue;
				}
				else if (i < argc && (strcmp(argv[i], "png16") == 0))
				{
					noGUIdata.imageFormat = imgFormatPNG16;
					printf("commandline: PNG 16-bit image format\n");
					continue;
				}
				else if (i < argc && (strcmp(argv[i], "png16alpha") == 0))
				{
					noGUIdata.imageFormat = imgFormatPNG16Alpha;
					printf("commandline: PNG 16-bit with alpha channel image format\n");
					continue;
				}
				else
				{
					printf("commandline: wrong image format\n");
				}
			}
			if (i < argc - 1 || (strcmp(argv[i], "-help") == 0))
			{
				printf("commandline: wrong parameters\n");
				printf("Syntax:\nmandelbulber [options...] [settings_file]\n");
				printf("options:\n");
				printf("  -nogui          - start program without GUI\n");
				printf("  -lowmem         - low memory usage mode\n");
				printf("  -flight         - render flight animation\n");
				printf("  -keyframe       - render keyframe animation\n");
				printf("  -start N        - start renderig from frame number N\n");
				printf("  -end            - rendering will end on frame number N\n");
				printf("  -format FORMAT  - image output format\n");
				printf("     jpg - JPEG format\n");
				printf("     png - PNG format\n");
				printf("     png16 - 16-bit PNG format\n");
				printf("     png16alpha - 16-bit PNG with alpha channel format\n");
				printf("[settings_file] - file with fractal settings (program also tries\nto find file in ./mandelbulber/settings directory)\n");
				printf("When settings_file is put as command argument then program will start in noGUI mode\n");
				result = false;
				break;
			}
			else
			{
				strcpy(noGUIdata.settingsFile, argv[i]);
				printf("commandline: settings file: %s\n", noGUIdata.settingsFile);
				noGUI = true;
			}
		}
	}
	return result;
}

void Params2InterfaceData(sParamRender *source)
{
	strcpy(Interface_data.file_destination, source->file_destination);
	strcpy(Interface_data.file_background, source->file_background);
	strcpy(Interface_data.file_envmap, source->file_envmap);
	strcpy(Interface_data.file_lightmap, source->file_lightmap);
	strcpy(Interface_data.file_path, source->file_path);
	strcpy(Interface_data.file_keyframes, source->file_keyframes);
	strcpy(Interface_data.file_sound, source->file_sound);
}

void InterfaceData2Params(sParamRender *dest)
{
	strcpy(dest->file_destination, Interface_data.file_destination);
	strcpy(dest->file_background, Interface_data.file_background);
	strcpy(dest->file_envmap, Interface_data.file_envmap);
	strcpy(dest->file_lightmap, Interface_data.file_lightmap);
	strcpy(dest->file_path, Interface_data.file_path);
	strcpy(dest->file_keyframes, Interface_data.file_keyframes);
	strcpy(dest->file_sound, Interface_data.file_sound);
}

