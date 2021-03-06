
/* NOTE: I am not using the uGui widgets and windows.
 * I wanted to be able to show a video, so I did my own.
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

#include "strutils.h"
#include "tiger_gl.h"
#include "ugui.h"

extern TglScreen *_ts;
extern TglInfo *_tglInfo;

int _widgetId = 0;

TGLBITMAP *upButton = NULL;
TGLBITMAP *downButton = NULL;

extern int twQue;

extern TwList *_twWidgets;
extern void _addAreaW(TglWidget *tw);

void _paintButton(TglWidget *tw, bool flag);
void _paintCheckbox(TglWidget *tw, bool flag);
void _paintRadio(TglWidget *tw, bool flag);
//void _paintTextbox(TglWidget *tw, bool flag);
void _paintProgressBar(TglWidget *tw, bool flag);
void _paintSpinner(TglWidget *tw, bool flag);
//void _paintListbox(TglWidget *tw, bool flag);

/* tglIsInside - Is x and Y inside the widget. */
int tglIsInside(TglWidget *tw, uint16_t x, uint16_t y) {

	if (x >= tw->x && x < (tw->x + tw->width) && y >= tw->y && y < (tw->y + tw->height))
		return 1;
	
	return 0;
}

/* tglWidgetRegisterV - Registers widgets with the library.
 *	This keeps track of widgets being displayed.
 *	Normally called by the tglWidgetRegister #define in tiger_gl.h
 *  *** NOTE: A widget must be registered to be displayed. ***
 */
int tglWidgetRegisterV(int count, ...) {
	va_list args;
	va_start(args, count);

	for (int i = 0; i < count; i++) {
		TglWidget *tw = va_arg(args, TglWidget *);
		tw->isRegistered = true;

		if (tw->paintWidget != NULL)
			tw->paintWidget(tw, 0);


		twAdd(_twWidgets, tw);
	}

	va_end(args);

	return 0;
}

/* tglWidgetSetData - Sets the user data of a widget.
 *	This is normally a string value but you can use other data types
 *	but you will have to handle the convertion your self.
 */
void tglWidgetSetData(TglWidget *tw, char * data) {

	if (data == NULL || tw == NULL)
		return;
	
	if (tw->data != NULL)
		free(tw->data);

	tw->data = strdup(data);
}

/* tglWidgetgetData - Retrives the user data of the widget. */
char *tglWidgetGetData(TglWidget *tw) {
	if (tw == NULL)
		return NULL;

	return tw->data;
}

/* tglWidgetEvent - This function is called by the tglTouchGetEvent() function
 *	if a touch event is detected.
 *	Normally not called by anyone else.
 */
void tglWidgetEvent(uint16_t x, uint16_t y, uint16_t p, uint16_t t, uint16_t c) {
	
	int flag = 0;
	// uint16_t direction = 0;
	ItemType it;

	TwData *h = _twWidgets->head;

	while(h != NULL) {
		TglWidget *tws = h->tw;
		if (tglIsInside(tws, x, y)) {

			if (tws->widgetType == WIDGET_BUTTON) {
				if ((t & TOUCH_START) == TOUCH_START) {
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
						_paintButton(tws, true);
						it.p = (char *)tws;
						cqAdd(twQue, &it);
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
						_paintButton(tws, true);
						it.p = (char *)tws;
						cqAdd(twQue, &it);
					}
				}
			} else if (tws->widgetType == WIDGET_SPINNER) {
				if ((t & TOUCH_START) == TOUCH_START) {
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
						if (tws->selected < (tws->cnt - 1))
							tws->selected++;
						else
							tws->selected = 0;
						_paintSpinner(tws, true);
						it.p = (char *)tws;
						cqAdd(twQue, &it);
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
						if (tws->selected < (tws->cnt - 1))
							tws->selected++;
						else
							tws->selected = 0;
						_paintSpinner(tws, true);
						it.p = (char *)tws;
						cqAdd(twQue, &it);
					}
				}
#if(0)
			} else if (tws->widgetType == WIDGET_LISTBOX) {
				if ((t & TOUCH_START) == TOUCH_START) {
					_paintListbox(tws, true);
					if (tws->touchAction == TOUCH_DOWN) {
						flag = 1;
					}
				} else if ((t & TOUCH_STOP) == TOUCH_STOP) {
					_paintListbox(tws, false);
					if (tws->touchAction == TOUCH_UP) {
						flag = 1;
					}
				} else if (t == (TOUCH_X | TOUCH_Y)) {
				}
#endif
			} else if (tws->widgetType == WIDGET_CHECKBOX) {
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
			} else if (tws->widgetType == WIDGET_RADIO) {
				if ((t & TOUCH_START) == TOUCH_START) {
					tws->checked = tws->checked == true ? false : true;
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
			} else if (tws->widgetType == WIDGET_RADIO) {
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
		} else {
			// A move event in the x or y direction.
			// if (t == (TOUCH_X | TOUCH_Y) && tws->widgetType == WIDGET_LISTBOX) {
			// 	if (y > oldY)
			// 		direction = SCROLL_UP;
			// 	else if ((y < oldY)
			// 		direction = SCROLL_DOWN;
			// 	oldY = y;
			// }
		}
		h = h->next;
	}
}

/* tglWidgetDelete - Deletes a widget and frees its memory.
 *	You must have special care when calling this function to delete
 *	a widget because it can have far reaching effects.
 *	I normally never call this function.
 */
void tglWidgetDelete(TglWidget *tw) {
	if (tw == NULL)
		return;
	
	if (tw->text != NULL)
		free(tw->text);

	if (tw->iconName != NULL)
		free(tw->iconName);
	
	if (tw->strList != NULL)
		free(tw->strList);

	if (tw->icon != NULL)
		FreeImage_Unload((FIBITMAP *)tw->icon);
	
	free(tw);
}

/* tglWidgetImage - Creates widget that an image can be displayed to.
 *	x and y are top left corner.
 */
TglWidget *tglWidgetImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
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

/* tglWidgetAddCallback - Adds a callback function to a widget.
 *	action - is whether you want the callback to occur at touch or release.
 *  TOUCH_DOWN = touch, TOUCH_UP = release
 *	Currently only one callback per widget.
 */
void tglWidgetAddCallback(TglWidget *tw, void (*eCallback)(struct _tglWidget_ *tw, uint16_t x, uint16_t y, uint16_t p), TouchAction action) {
	tw->eCallback = eCallback;
	tw->touchAction = action;
}

/* tglWidgetAddIcon - Adds an icon image to the widget if it supports it.
 *	icnName is a path to an image file.
 *	Any image supported by the FreeImage libray is supported.
 */
void tglWidgetAddIcon(TglWidget *tw, char *iconName) {
	if (tw == NULL)
		return;
	if (iconName == NULL)
		return;
	
	tw->iconName = strdup(iconName);
	
	tw->icon = tglImageLoad(iconName);
}

/* _paintButton - Function to paint a button widget.
 *	 I could have one paint function for all widget types but I
 *	 think that code looks ugly and this is simpler.
 */
void _paintButton(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	if (flag == true) {
		tw->isPressed = true;
		tglDrawFillRoundRect(tw->x, tw->y, tw->width, tw->height, 5, TGL_COLOR_DARKGRAY);
		tglDrawRoundRect(tw->x+7, tw->y+7, tw->width-14, tw->height-14, 8, tw->fgColor);
		tglDrawFillRoundRect(tw->x+8, tw->y+8, tw->width-16, tw->height-16, 5, tw->bgColor + 0x202020);
	} else {
		tglDrawFillRoundRect(tw->x, tw->y, tw->width, tw->height, 5, tw->bgColor);
		tglDrawRoundRect(tw->x+7, tw->y+7, tw->width-14, tw->height-14, 8, tw->fgColor);
		tglDrawFillRoundRect(tw->x+8, tw->y+8, tw->width-16, tw->height-16, 5, tw->bgColor + 0x202020);
		tw->isPressed = false;
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

		// UG_PrintFontInfo(gf);

		int sw = 0;
		int len = strlen(tw->text);
		if (gf->widths == NULL) {
			sw = (gf->char_width * len);
		} else {
			for (int i = 0; i < len; ++i) {
				sw += gf->widths[(int)tw->text[i] - gf->start_char];
			}
		}
		int xp = 0;
		int yp = 0;

		xp = ((tw->width - sw) / 2) + tw->x;
		if (tw->iconName) {
			yp = ((tw->y + tw->height) - gf->char_height) - 13;
		} else {
			yp = ((tw->height - gf->char_height) / 2) + tw->y;
		}

		tglDrawSetClipRegion(tw->x+8, tw->y+8, tw->width-16, tw->height-16);
		if(flag)
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, 0, true);
		else
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, 0, true);
		tglDrawUnsetClipRegion();

	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
		_addAreaW(tw);

}

/* tglWidgetSetButtonText - Changes the button text and calls the _paintButton function. */
void tglWidgetSetButtonText(TglWidget *tw, char *text) {
	 if (text != NULL) {
        if (tw->text != NULL)
            free(tw->text);
        tw->text = strdup(text);
    } else {
        if (tw->text != NULL)
            free(tw->text);
        tw->text = NULL;
    }
    _paintButton(tw, false);
	if (tw->isRegistered)
		_addAreaW(tw);
}

#if(0)
void tglWidgetTouched(TglWidget *tw) {
	tw->paintWidget(tw, 0);

	struct timespec t;

	t.tv_sec = 0;
	t.tv_nsec = 250000000;

	nanosleep(&t, NULL);

	tw->paintWidget(tw, 1);
}
#endif

/* tglWidgetButton - Create a button */
TglWidget *tglWidgetButton(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_BUTTON;
	tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
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
		
		tglDrawSetClipRegion(tw->x+4, tw->y+4, tw->width-9, tw->height-9);
		if (tw->hasFocus == true)
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, TGL_COLOR_LIGHTGREEN, true);
		else
			tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor, true);
		tglDrawUnsetClipRegion();
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	_addAreaW(tw);
}

void tglWidgetSetTextboxFocus(TglWidget *tw, bool hasFocus) {
	tw->hasFocus = hasFocus;

	if (tw->isRegistered)
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
	if (tw->isRegistered)
		_addAreaW(tw);
}

TglWidget *tglWidgetTextbox(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_TEXTBOX;
	tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
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

/* _paintLabel - Paints a label type widget. */
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
		
		tglDrawSetClipRegion(tw->x, tw->y, tw->width, tw->height);
		tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor, true);
		tglDrawUnsetClipRegion();
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetSetFont - Sets the font used by the widget. */
void tglWidgetSetFont(TglWidget *tw, char *fontName) {
	tw->textFont = UG_FontSelectByName(fontName);
}

/* tglWidgetSetTextColor - Sets the text color used by the widget. */
void tglWidgetSetTextColor(TglWidget *tw, uint32_t c) {
	tw->txtColor = c;
}

/* tglWidgetSetFgColor - Sets the foreground color used by the widget. */
void tglWidgetSetFgColor(TglWidget *tw, uint32_t fgColor) {
	tw->fgColor = fgColor;
}

/* tglWidgetSetBgColor - Sets the background color used by the widget. */
void tglWidgetSetBgColor(TglWidget *tw, uint32_t bgColor) {
	tw->bgColor = bgColor;
}

/* tglWidgetSetFgBgColor - Sets both foreground and  background color used by the widget. */
void tglWidgetSetFgBgColor(TglWidget *tw, uint32_t fgColor, uint32_t bgColor) {
	tglWidgetSetFgColor(tw, fgColor);
	tglWidgetSetBgColor(tw, bgColor);
}

/* tglWidgetSetLabelText - Set label text and calls the _paintLabel function. */
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
	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetLabel - Creates a label widget. */
TglWidget *tglWidgetLabel(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	TglWidget *tw = NULL;

	tw = (TglWidget *)calloc(1, sizeof(TglWidget));
	if (tw == NULL)
		return NULL;

	tw->widgetType = WIDGET_LABEL;
	tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
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

	return tw;
}

/* _paintCheckbox - Paints a checkbox widget. */
void _paintCheckbox(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, tw->bgColor);

	tglDrawRoundRect(tw->x+3, tw->y+3, tw->height-18, tw->height-6, 8, tw->fgColor);

	if (flag == true) {
		tglDrawLine(tw->x+10, tw->y+18, tw->x+14, (tw->y + tw->height) - 18, tw->fgColor);
		tglDrawLine(tw->x+14, (tw->y + tw->height) - 18, tw->x+24, tw->y+10, tw->fgColor);
		tglDrawLine(tw->x+11, tw->y+17, tw->x+15, (tw->y + tw->height) - 19, tw->fgColor);
		tglDrawLine(tw->x+15, (tw->y + tw->height) - 17, tw->x+25, tw->y+10, tw->fgColor);
	} else {
		tglDrawLine(tw->x+10, tw->y+18, tw->x+14, (tw->y + tw->height) - 18, tw->bgColor);
		tglDrawLine(tw->x+14, (tw->y + tw->height) - 18, tw->x+24, tw->y+10, tw->bgColor);
		tglDrawLine(tw->x+11, tw->y+17, tw->x+15, (tw->y + tw->height) - 19, tw->bgColor);
		tglDrawLine(tw->x+15, (tw->y + tw->height) - 17, tw->x+25, tw->y+10, tw->bgColor);
	}

	if (tw->text != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		int xp = tw->x + tw->height + 4;
		int yp = ((tw->height - gf->char_height) / 2) + tw->y;
		
		tglDrawSetClipRegion(xp, yp, tw->width, tw->height);
		tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor, true);
		tglDrawUnsetClipRegion();
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetSetCheckboxText - Sets the text of a checkbox widget. */
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
	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetCheckbox - Create a checkbox widget. */
TglWidget *tglWidgetCheckbox(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_CHECKBOX;
    tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
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

/* _paintRadio = Paints a radio button widget. */
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
		
		tglDrawSetClipRegion(xp, yp, tw->width-24, tw->height);
		tglDrawPutString(xp, yp, tw->text, tw->fgColor, tw->bgColor, true);
		tglDrawUnsetClipRegion();
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetSetRadioText - Sets the text of a radio button widget. */
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
	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetSetRadioGroup - Sets the group the widget belongs to.
 *	All radio buttons of the same group call only have one radio button selected
 *	at any given time.
 */
void tglWidgetSetRadioGroup(TglWidget *tw, uint16_t groupId) {
	tw->groupId = groupId;
}

/* tglWidgetSetSelected - Sets the item selected within a widget. */
void tglWidgetSetSelected(TglWidget *tw, bool selected) {
	tw->checked = selected;
	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetRadio - Creates a radio button. */
TglWidget *tglWidgetRadio(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_RADIO;
    tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
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

/* _paintProgressBar - Paints a progress bar widget. */
void _paintProgressBar(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, TGL_COLOR_WHITE);
	tglDrawRoundRect(tw->x+4, tw->y+4, tw->width-10, tw->height-10, 10, tw->fgColor);

	if (tw->pbNum > 0) {
		int n = tw->width-10;			// length of widget minus borders.
		double w = (double)n / (double)tw->pbMax;
		int p = (int)((double)tw->pbNum * w);
		tglDrawFillRoundRect(tw->x+5, tw->y+5, p, tw->height-12, 10, tw->pbColor);
	}

	if (tw->pbText == true) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		char textNum[32];
		sprintf(textNum, "%d%%", tw->pbNum);

		int sw =0;
		int len = strlen(textNum);
		if (gf->widths == NULL) {
			sw = (gf->char_width * len);
		} else {
			for ( int i = 0; i < len; ++i) {
				sw += gf->widths[textNum[i] - gf->start_char];
			}
		}
		int xp = ((tw->width - sw) / 2) + tw->x;
		int yp = ((tw->height - gf->char_height) / 2) + tw->y;

		int txtColor = 0xffffff - tw->pbColor;

		tglDrawPutString(xp, yp, textNum, txtColor, 0, true);
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
	_addAreaW(tw);
}

/* tglWidgetSetProgressBarColor - Sets the color used for progress. */
void tglWidgetSetProgressBarColor(TglWidget *tw, uint32_t c) {
	tw->pbColor = c;
}

/* tglWidgetSetProgressBarNum - Sets the progress value. */
void tglWidgetSetProgressBarNum(TglWidget *tw, uint16_t num) {
	tw->pbNum = num;
}

/* tglWidgetProgressBar - Creates a progress bar widget. */
TglWidget *tglWidgetProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool pbText, uint32_t c) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_PROGRESSBAR;
    tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
	tw->pbText = pbText;
	tw->pbNum = 0;
	tw->pbMax = 100;
	tw->fgColor = TGL_COLOR_BLACK;
	tw->bgColor = 0;
	tw->pbColor = c;
    tw->x = x;
    tw->y = y;
    tw->width = width;
    tw->height = height;
    tw->paintWidget = _paintProgressBar;

    return tw;
}

/* toStringArray - Converts a comma speerated string list into a string array. */
int toStringArray(TglWidget *tw, char *str) {
	int longest = 0;

	tw->cnt = countArgs(str);

	char *args[tw->cnt + 1];

	char *s = strdup(str);		// parse modifies string.

	int r = parse(s, ",", args, tw->cnt + 1);

	if (r <= 0) {
		printf("parse returned error %d\n", r);
		free(s);
		return 0;
	}
	
	for (int i = 0; i < r; i++) {
		int x = strlen(args[i]);
		if (x > longest)
			longest = x;
	}

	tw->len = longest + 1;

	tw->strList = (char *)calloc(tw->cnt, tw->len);
	char *p = tw->strList;

	for (int i = 0; i < r; i++) {
		strcpy(p, args[i]);
		p += tw->len;
	}

	free(s);

	return r;
}

/* tglWidgetSetSelection - Sets the selected item in a Spinner widget. */
void tglWidgetSetSelection(TglWidget *tw, uint16_t num) {

	if (tw->widgetType != WIDGET_SPINNER && tw->widgetType != WIDGET_LISTBOX) {
		return;
	}

	if (num < 0 || num >= tw->cnt) {
		tw->selected = 0;
	} else {
		tw->selected = num;
	}

	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetGetSelection - Get the selected item in a Spinner widget. */
char *tglWidgetGetSelection(TglWidget *tw) {
	char *s = NULL;

	if (tw->widgetType == WIDGET_SPINNER) {
		s = (char *)(tw->strList + (tw->len * tw->selected));
	}

	return s;
}

/* tglWidgetSetSpinnerList - Sets the spinners list. */
void tglWidgetSetSpinnerList(TglWidget *tw, char *spList) {

	if (spList != NULL) {
		if (tw->strList != NULL)
			free(tw->strList);

		toStringArray(tw, spList);
		if (tw->selected >= tw->cnt)
			tw->selected = 0;
	}

	if (tw->isRegistered)
		_addAreaW(tw);
}

/* _paintSpinner - Paints a spinner type widget. */
void _paintSpinner(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	printf("flag %d\n", flag);

	if (flag)
		tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, TGL_COLOR_DARKGRAY);
	else
		tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, TGL_COLOR_WHITE);
	tglDrawRect(tw->x+5, tw->y+5, tw->width-12, tw->height-12, tw->fgColor);
	tglDrawFillRect(tw->x+6, tw->y+6, tw->width-14, tw->height-14, TGL_COLOR_LIGHTERGRAY);

	if (tw->strList != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		UG_FONT *gf = tw->textFont;

		int xp = tw->x + 12;
		int yp = ((tw->height - gf->char_height) / 2) + tw->y;

		tglDrawSetClipRegion(tw->x+6, tw->y+6, tw->width-14, tw->height-14);		
		// printf("selected %s\n", (char *)(tw->strList + (tw->len * tw->selected)));
		tglDrawPutString(xp, yp, (char *)(tw->strList + (tw->len * tw->selected)), tw->fgColor, 0, true);
		tglDrawUnsetClipRegion();
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
		_addAreaW(tw);
}

/* tglWidgetSpinner - Creates a spinnet type widget.
 *	spList is a comma seperated list of strings, ie: "kelly,was here,and,gone." 4 parts
 */
TglWidget *tglWidgetSpinner(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *spList) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_SPINNER;
    tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
	tw->pbNum = 0;
	tw->pbMax = 100;
	if (spList != NULL) {
		toStringArray(tw, spList);
	}
	tw->fgColor = TGL_COLOR_BLACK;
	tw->bgColor = TGL_COLOR_WHITE;
    tw->x = x;
    tw->y = y;
    tw->width = width;
    tw->height = height;
    tw->paintWidget = _paintSpinner;

    return tw;
}

#if(0)
void _paintListbox(TglWidget *tw, bool flag) {

	tglSetAutoUpdate(TGL_NO_AUTOUPDATE);

	tglDrawFillRect(tw->x, tw->y, tw->width, tw->height, TGL_COLOR_WHITE);
	tglDrawRect(tw->x+5, tw->y+5, tw->width-12, tw->height-12, tw->fgColor);

	uint16_t upX = tw->x + tw->width - 13;
	uint16_t upY = tw->y+5;
	uint16_t upWidth = 12;
	uint16_t upHeight = (tw->height-12);

	tglDrawRoundRect(upX, upY, upWidth, upHeight, 10, TGL_COLOR_BLACK);
	tglDrawFillRoundRect(upX+2, upY+2, upWidth-4, upHeight-4, 5, TGL_COLOR_LIGHTERGRAY);

	UG_FONT *gf = NULL;

	if (tw->strList != NULL) {
		if (tw->textFont == NULL)
			tw->textFont = UG_GetFont("FONT_6x10");

		UG_SetFont(tw->textFont);

		gf = tw->textFont;

		// UG_PrintFontInfo(gf);

		int padding = 9;

		int cnt = (tw->height-12) / (gf->char_height + padding);

		int left = tw->height - 12;
		int xp = tw->x+9;
		int yp = tw->y+9;
		for (int i = tw->position; i < tw->cnt && cnt >= 0; i++) {
			if (left < (gf->char_height + padding))
				tglDrawSetClipRegion(xp, yp, tw->width-25, ((gf->char_height + padding + 4) - abs(left)));		
			else
				tglDrawSetClipRegion(xp, yp, tw->width-25, gf->char_height);		
			tglDrawPutString(xp, yp, (char *)(tw->strList + (tw->len * i)), tw->fgColor, 0, true);
			tglDrawUnsetClipRegion();

			tglDrawLine(xp, yp + gf->char_height + (padding / 2) + 1, xp + (tw->width - 25), yp + gf->char_height + (padding / 2) + 1, TGL_COLOR_BLACK);

			yp += gf->char_height + padding;
			left -= gf->char_height + padding;
			--cnt;
		}
	}

	tglSetAutoUpdate(TGL_AUTOUPDATE);

	if (tw->isRegistered)
		_addAreaW(tw);
}

void tglWidgetSetListboxPosition(TglWidget *tw, uint16_t position) {
	tw->position = position;

	if (tw->isRegistered)
		_addAreaW(tw);
}

TglWidget *tglWidgetListbox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *strList) {
	 TglWidget *tw = NULL;

    tw = (TglWidget *)calloc(1, sizeof(TglWidget));
    if (tw == NULL)
        return NULL;

    tw->widgetType = WIDGET_LISTBOX;
    tw->widgetId = _widgetId++;
	tw->txtColor = TGL_COLOR_BLACK;
	tw->selected = 0;
	if (strList != NULL) {
		toStringArray(tw, strList);
	}
	tw->fgColor = TGL_COLOR_BLACK;
	tw->bgColor = TGL_COLOR_WHITE;
    tw->x = x;
    tw->y = y;
    tw->width = width;
    tw->height = height;
    tw->paintWidget = _paintListbox;

    return tw;
}
#endif
