
/* NOTE: I am not using the uGui widgets.
 * To me they seem too confusing but I am not too bright.
 * And I wanted to be able to show a video, so I did my own.
 * KW
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <FreeImage.h>

#include "tiger_gl.h"
#include "ugui.h"

extern TglScreen *_ts;
extern TglInfo *_tglInfo;

int _widgetId = 0;

extern TwList *_twWidgets;
extern void _addAreaW(TglWidget *tw);

void _paintButton(TglWidget *tw, bool flag);
void _paintCheckbox(TglWidget *tw, bool flag);
void _paintRadio(TglWidget *tw, bool flag);
//void _paintTextbox(TglWidget *tw, bool flag);

int tglIsInside(TglWidget *tw, int x, int y) {

	if (x >= tw->x && x < (tw->x + tw->width) && y >= tw->y && y < (tw->y + tw->height))
		return 1;
	
	return 0;
}

int tglWidgetRegisterV(int count, ...) {
	va_list args;
	va_start(args, count);

	for (int i = 0; i < count; i++) {
		TglWidget *tw = va_arg(args, TglWidget *);

		if (tw->paintWidget != NULL)
			tw->paintWidget(tw, 0);

		twAdd(_twWidgets, tw);
	}

	va_end(args);

	return 0;
}

void tglWidgetSetData(TglWidget *tw, char * data) {

	if (data == NULL || tw == NULL)
		return;
	
	if (tw->data != NULL)
		free(tw->data);

	tw->data = strdup(data);
}

char *tglWidgetGetData(TglWidget *tw) {
	if (tw == NULL)
		return NULL;

	return tw->data;
}

void tglWidgetEvent(int x, int y, int p, int t) {
	
	int flag = 0;

	TwData *h = _twWidgets->head;

	while(h != NULL) {
		TglWidget *tws = h->tw;
		if (tglIsInside(tws, x, y)) {

			if (tws->widgetType == WIDGET_BUTTON) {
				if ((t & TOUCH_START) == TOUCH_START) {
					_paintButton(tws, true);
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					_paintButton(tws, false);
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
					}
				}
			}

			if (tws->widgetType == WIDGET_CHECKBOX) {
				if ((t & TOUCH_START) == TOUCH_START) {
					tws->checked = tws->checked == true ? false : true;
					_paintCheckbox(tws, tws->checked);
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					_paintCheckbox(tws, tws->checked);
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
					}
				}
			}

			if (tws->widgetType == WIDGET_CHECKBOX) {
				if ((t & TOUCH_START) == TOUCH_START) {
					tws->checked = tws->checked == true ? false : true;
					_paintCheckbox(tws, tws->checked);
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					_paintCheckbox(tws, tws->checked);
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
					}
				}
			}

#if(0)
			if (tws->widgetType == WIDGET_TEXTBOX) {
				if ((t & TOUCH_START) == TOUCH_START) {
					TwData *h2 = _twWidgets->head;

					while(h2 != NULL) {
						if (h2->tw->widgetType == WIDGET_TEXTBOX) {
							h2->tw->hasFocus = false;
							_paintTextbox(h2->tw, h2->tw->hasFocus);
						}
						h2 = h2->next;
					}
					tws->hasFocus = true;
					_paintTextbox(tws, tws->checked);
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					_paintTextbox(tws, tws->checked);
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
					}
				}
			}
#endif

			if (tws->widgetType == WIDGET_RADIO) {
				if ((t & TOUCH_START) == TOUCH_START) {
					TwData *h2 = _twWidgets->head;

					while(h2 != NULL) {
						if (tws->groupId == h2->tw->groupId) {
							h2->tw->checked = false;
							_paintRadio(h2->tw, h2->tw->checked);
						}
						h2 = h2->next;
					}
					tws->checked = true;

					_paintRadio(tws, tws->checked);
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					_paintRadio(tws, tws->checked);
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
					}
				}
			}

			// printf("flag = %d, checked %d\n", flag, tws->checked);
			if (flag && tws->eCallback != NULL) {
				if (tws->eCallback)
					tws->eCallback(tws, x, y, p);
			}
			break;
		}
		h = h->next;
	}
}

void tglWidgetDelete(TglWidget *tw) {
	if (tw == NULL)
		return;
	
	if (tw->text != NULL)
		free(tw->text);

	if (tw->iconName != NULL)
		free(tw->iconName);

	if (tw->icon != NULL)
		FreeImage_Unload((FIBITMAP *)tw->icon);
	
	free(tw);
}

TglWidget *tglWidgetImage(int x, int y, int width, int height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_IMAGE;
	tw->widgetId = _widgetId++;
	tw->x = x;
	tw->y = y;
	tw->width = width;
	tw->height = height;

	return tw;
}

// action - is whether you want the callback to occur at touch or release.
// TOUCH_DOWN = touch, TOUCH_UP = release
void tglWidgetAddCallback(TglWidget *tw, void (*eCallback)(struct _tglWidget_ *tw, int x, int y, int p), TouchAction action) {
	tw->eCallback = eCallback;
	tw->touchAction = action;
}

void tglWidgetAddIcon(TglWidget *tw, char *iconName) {
	if (tw == NULL)
		return;
	if (iconName == NULL)
		return;
	
	tw->iconName = strdup(iconName);
	
	tw->icon = tglImageLoad(iconName);
}

void _paintButton(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	if (flag == true) {
		tglDrawFillRoundRect(tw->x, tw->y, tw->width, tw->height, 5, TGL_COLOR_DARKGRAY);
		tglDrawRoundRect(tw->x+7, tw->y+7, tw->width-14, tw->height-14, 8, tw->fgColor);
		tglDrawFillRoundRect(tw->x+8, tw->y+8, tw->width-16, tw->height-16, 5, tw->bgColor + 0x202020);
	} else {
		tglDrawFillRoundRect(tw->x, tw->y, tw->width, tw->height, 5, tw->bgColor);
		tglDrawRoundRect(tw->x+7, tw->y+7, tw->width-14, tw->height-14, 8, tw->fgColor);
		tglDrawFillRoundRect(tw->x+8, tw->y+8, tw->width-16, tw->height-16, 5, tw->bgColor + 0x202020);
	}

	if (tw->iconName) {
		int iw = tglImageGetWidth(tw->icon);	
		int ih = tglImageGetHeight(tw->icon);	
		int xp = 0;
		int yp = 0;

		xp = ((tw->width - iw) / 2) + tw->x;
		if (tw->text != NULL) {
			yp = ((tw->height - ih) / 3) + tw->y;
		} else {
			yp = ((tw->height - ih) / 2) + tw->y;
		}

		// Replace transparent color with background color.
		TGLBITMAP *t = tglImageComposite(tw->icon, tw->bgColor + 0x202020);
		TGLBITMAP *iconImg = tglImageConvertTo32(t);
		tglDrawImage(xp, yp, iconImg, 0);
		tglImageDelete(t);
		tglImageDelete(iconImg);
	}

	if (tw->text != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;
		int sw = (gf->char_width * strlen(tw->text));
		int xp = 0;
		int yp = 0;

		xp = ((tw->width - sw) / 2) + tw->x;
		if (tw->iconName) {
			yp = ((tw->y + tw->height) - gf->char_height) - 13;
		} else {
			yp = ((tw->height - gf->char_height) / 2) + tw->y;
		}
		
		if(flag)
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor + 0x202020);
		else
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor + 0x202020);

	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	_addAreaW(tw);

}

void tglWidgetTouched(TglWidget *tw) {
	tw->paintWidget(tw, 0);

	struct timespec t;

	t.tv_sec = 0;
	t.tv_nsec = 250000000;

	nanosleep(&t, NULL);

	tw->paintWidget(tw, 1);
}

TglWidget *tglWidgetButton(char *text, int x, int y, int width, int height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_BUTTON;
	tw->widgetId = _widgetId++;
	tw->x = x;
	tw->y = y;
	tw->width = width;
	tw->height = height;
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);

		tw->text = strdup(text);
	}
	tw->paintWidget = _paintButton;

	return tw;
}

#if(0)
void _paintTextbox(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, tw->bgColor);
	tglDrawRect(tw->x+3, tw->y+3, tw->width-7, tw->height-7, tw->fgColor);

	if (tw->hasFocus == true) {
		tglDrawFillRect(tw->x+4, tw->y+4, tw->width-9, tw->height-9, TGL_COLOR_LIGHTGREEN);
	} else {
		tglDrawFillRect(tw->x+4, tw->y+4, tw->width-9, tw->height-9, tw->bgColor);
	}

	if (tw->text != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		int xp = tw->x + 6;
		int yp = ((tw->height - gf->char_height) / 2) + (tw->y + (gf->char_height / 2));
		
		if (tw->hasFocus == true)
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, TGL_COLOR_LIGHTGREEN);
		else
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor);
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	_addAreaW(tw);
}

void tglWidgetSetTextboxFocus(TglWidget *tw, bool hasFocus) {
	tw->hasFocus = hasFocus;

	_addAreaW(tw);
}

void tglWidgetSetTextboxText(TglWidget *tw, char *text) {
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);
		tw->text = strdup(text);
	} else {
		if (tw->text != NULL)
			free(tw->text);
		tw->text = NULL;
	}
	_paintTextbox(tw, false);
	_addAreaW(tw);
}

TglWidget *tglWidgetTextbox(char *text, int x, int y, int width, int height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_TEXTBOX;
	tw->widgetId = _widgetId++;
	tw->x = x;
	tw->y = y;
	tw->width = width;
	tw->height = height;
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);

		tw->text = strdup(text);
	}
	tw->paintWidget = _paintTextbox;

	return tw;
}
#endif

void _paintLabel(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, tw->bgColor);

	if (tw->text != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		int xp = tw->x + 4;
		int yp = ((tw->height - gf->char_height) / 2) + tw->y;
		
		tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor);
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	_addAreaW(tw);
}

void tglWidgetSetFont(TglWidget *tw, char *fontName) {
	tw->textFont = UG_FontSelectByName(fontName);
}

void tglWidgetSetFgColor(TglWidget *tw, int fgColor) {
	tw->fgColor = fgColor;
}

void tglWidgetSetBgColor(TglWidget *tw, int bgColor) {
	tw->bgColor = bgColor;
}

void tglWidgetSetFgBgColor(TglWidget *tw, int fgColor, int bgColor) {
	tglWidgetSetFgColor(tw, fgColor);
	tglWidgetSetBgColor(tw, bgColor);
}

void tglWidgetSetLabelText(TglWidget *tw, char *text) {
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);
		tw->text = strdup(text);
	} else {
		if (tw->text != NULL)
			free(tw->text);
		tw->text = NULL;
	}
	_paintLabel(tw, false);
	_addAreaW(tw);
}

TglWidget *tglWidgetLabel(char *text, int x, int y, int width, int height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_LABEL;
	tw->widgetId = _widgetId++;
	tw->x = x;
	tw->y = y;
	tw->width = width;
	tw->height = height;
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);

		tw->text = strdup(text);
	}
	tw->paintWidget = _paintLabel;

	// tglDrawFillRect(x, y, width, height, tw->bgColor);

	return tw;
}

void _paintCheckbox(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, tw->bgColor);

	tglDrawRoundRect(tw->x+3, tw->y+3, tw->height-18, tw->height-6, 8, tw->fgColor);

	if (flag == true) {
		tglDrawLine(tw->x+10, tw->y+18, tw->x+14, (tw->y + tw->height) - 18, tw->fgColor);
		tglDrawLine(tw->x+14, (tw->y + tw->height) - 18, tw->x+24, tw->y+10, tw->fgColor);
		tglDrawLine(tw->x+11, tw->y+17, tw->x+15, (tw->y + tw->height) - 19, tw->fgColor);
		tglDrawLine(tw->x+15, (tw->y + tw->height) - 17, tw->x+25, tw->y+10, tw->fgColor);
		// tglDrawFillRect(tw->x+8, tw->y+8, tw->height-16, tw->height-16, tw->fgColor);
	} else {
		tglDrawLine(tw->x+10, tw->y+18, tw->x+14, (tw->y + tw->height) - 18, tw->bgColor);
		tglDrawLine(tw->x+14, (tw->y + tw->height) - 18, tw->x+24, tw->y+10, tw->bgColor);
		tglDrawLine(tw->x+11, tw->y+17, tw->x+15, (tw->y + tw->height) - 19, tw->bgColor);
		tglDrawLine(tw->x+15, (tw->y + tw->height) - 17, tw->x+25, tw->y+10, tw->bgColor);
		// tglDrawFillRect(tw->x+8, tw->y+8, tw->height-16, tw->height-16, tw->bgColor);
	}

	if (tw->text != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		int xp = tw->x + tw->height + 4;
		int yp = ((tw->height - gf->char_height) / 2) + tw->y;
		
		tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor);
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	_addAreaW(tw);
}

void tglWidgetSetCheckboxText(TglWidget *tw, char *text) {
	 if (text != NULL) {
        if (tw->text != NULL)
            free(tw->text);
        tw->text = strdup(text);
    } else {
        if (tw->text != NULL)
            free(tw->text);
        tw->text = NULL;
    }
    _paintCheckbox(tw, false);
    _addAreaW(tw);
}

TglWidget *tglWidgetCheckbox(char *text, int x, int y, int width, int height) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_CHECKBOX;
    tw->widgetId = _widgetId++;
    tw->x = x;
    tw->y = y;
    tw->width = width;
    tw->height = height;
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);

		tw->text = strdup(text);
	}
    tw->paintWidget = _paintCheckbox;

    return tw;
}

void _paintRadio(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, tw->bgColor);

	tglDrawCircle(tw->x+13, tw->y+(tw->height/2), (tw->height/2) - 10, tw->fgColor);
	tglDrawCircle(tw->x+13, tw->y+(tw->height/2), (tw->height/2) - 11, tw->fgColor);

	if (flag == true) {
		tglDrawFillCircle(tw->x+13, tw->y+(tw->height/2), tw->height - 33, tw->fgColor);
	} else {
		tglDrawFillCircle(tw->x+13, tw->y+(tw->height/2), tw->height - 36, tw->bgColor);
	}

	if (tw->text != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		int xp = tw->x + (tw->height - 20) + 4;
		int yp = ((tw->height - gf->char_height) / 2) + tw->y;
		
		tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor);
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	_addAreaW(tw);
}

void tglWidgetSetRadioText(TglWidget *tw, char *text) {
	 if (text != NULL) {
        if (tw->text != NULL)
            free(tw->text);
        tw->text = strdup(text);
    } else {
        if (tw->text != NULL)
            free(tw->text);
        tw->text = NULL;
    }
    _paintRadio(tw, false);
    _addAreaW(tw);
}

void tglWidgetSetRadioGroup(TglWidget *tw, short groupId) {
	tw->groupId = groupId;
}

void tglWidgetSetSelected(TglWidget *tw, bool selected) {
	tw->checked = selected;
	_addAreaW(tw);
}

TglWidget *tglWidgetRadio(char *text, int x, int y, int width, int height) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_RADIO;
    tw->widgetId = _widgetId++;
    tw->x = x;
    tw->y = y;
    tw->width = width;
    tw->height = height;
	if (text != NULL) {
		if (tw->text != NULL)
			free(tw->text);

		tw->text = strdup(text);
	}
    tw->paintWidget = _paintRadio;

    return tw;
}
