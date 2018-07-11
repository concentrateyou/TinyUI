#include "../stdafx.h"
#include "TinyVisualCommon.h"

namespace TinyFramework
{
	namespace Windowless
	{
		LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
		{
			return (LONG)MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
		}
		LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
		{
			return (LONG)MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
		}
		LONG DXtoHimetricX(LONG dx, LONG xPerInch)
		{
			return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
		}
		LONG DYtoHimetricY(LONG dy, LONG yPerInch)
		{
			return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
		}

		const TinyString TinyVisualPropertyConst::NAME = TEXT("name");
		const TinyString TinyVisualPropertyConst::TEXT = TEXT("text");
		const TinyString TinyVisualPropertyConst::TOOLTIP = TEXT("tooltip");
		const TinyString TinyVisualPropertyConst::MAXSIZE = TEXT("maxsize");
		const TinyString TinyVisualPropertyConst::MINSIZE = TEXT("minsize");
		const TinyString TinyVisualPropertyConst::POSITION = TEXT("position");
		const TinyString TinyVisualPropertyConst::SIZE = TEXT("size");
		const TinyString TinyVisualPropertyConst::VISIBLE = TEXT("visible");
		const TinyString TinyVisualPropertyConst::ENABLE = TEXT("enable");
		const TinyString TinyVisualPropertyConst::CURSOR = TEXT("cursor");


		const TinyString TinyVisualPropertyConst::ORIENTATION = TEXT("orientation");

		const TinyString TinyVisualPropertyConst::PADDING = TEXT("padding");
		const TinyString TinyVisualPropertyConst::MARGIN = TEXT("margin");

		const TinyString TinyVisualPropertyConst::BACKGROUNDIMAGE = TEXT("background-image");
		const TinyString TinyVisualPropertyConst::BACKGROUNDCOLOR = TEXT("background-color");
		const TinyString TinyVisualPropertyConst::BACKGROUNDCENTER = TEXT("background-center");
		const TinyString TinyVisualPropertyConst::BACKGROUNDSIZE = TEXT("background-size");
		const TinyString TinyVisualPropertyConst::BACKGROUNDPOSITION = TEXT("background-position");

		const TinyString TinyVisualPropertyConst::BORDERWIDTH = TEXT("border-width");
		const TinyString TinyVisualPropertyConst::BORDERCOLOR = TEXT("border-color");
		const TinyString TinyVisualPropertyConst::BORDERSTYLE = TEXT("border-style");
		const TinyString TinyVisualPropertyConst::BORDERIMAGE = TEXT("border-image");
		const TinyString TinyVisualPropertyConst::BORDERCENTER = TEXT("border-center");

		const TinyString TinyVisualPropertyConst::FONTFAMILY = TEXT("font-family");
		const TinyString TinyVisualPropertyConst::FONTSIZE = TEXT("font-size");
		const TinyString TinyVisualPropertyConst::FONTWEIGHT = TEXT("font-weight");
		const TinyString TinyVisualPropertyConst::FONTSTYLE = TEXT("font-style");

		const TinyString TinyVisualPropertyConst::TEXTCOLOR = TEXT("text-color");
		const TinyString TinyVisualPropertyConst::TEXTALIGN = TEXT("text-align");
		const TinyString TinyVisualPropertyConst::TEXTDECORATION = TEXT("text-decoration");
		const TinyString TinyVisualPropertyConst::MULTILINE = TEXT("multiline");
		const TinyString TinyVisualPropertyConst::READONLY = TEXT("readonly");

		const TinyString TinyVisualPropertyConst::HORIZONTALALIGNMENT = TEXT("horizontal-alignment");
		const TinyString TinyVisualPropertyConst::VERTICALALIGNMENT = TEXT("vertical-alignment");
		const TinyString TinyVisualPropertyConst::VALUE = TEXT("value");
		const TinyString TinyVisualPropertyConst::OPTIONHIGHLIGHT = TEXT("highlight");

		const TinyString TinyVisualPropertyConst::IMAGENORMAL = TEXT("image-normal");
		const TinyString TinyVisualPropertyConst::IMAGEHIGHLIGHT = TEXT("image-highlight");
		const TinyString TinyVisualPropertyConst::IMAGEDOWN = TEXT("image-down");
		const TinyString TinyVisualPropertyConst::IMAGEPUSH = TEXT("image-push");

		const TinyString TinyVisualPropertyConst::IMAGE = TEXT("image");
		const TinyString TinyVisualPropertyConst::IMAGELIST = TEXT("imagelist");

		const TinyString TinyVisualPropertyConst::ARROWNORMAL = TEXT("arrow-normal");
		const TinyString TinyVisualPropertyConst::ARROWDOWN = TEXT("arrow-down");
		const TinyString TinyVisualPropertyConst::ARROWHIGHLIGHT = TEXT("arrow-highlight");
		const TinyString TinyVisualPropertyConst::ARROWPUSH = TEXT("arrow-push");

		const TinyString TinyVisualPropertyConst::ARROW1NORMAL = TEXT("arrow1-normal");
		const TinyString TinyVisualPropertyConst::ARROW1HIGHLIGHT = TEXT("arrow1-highlight");
		const TinyString TinyVisualPropertyConst::ARROW1DOWN = TEXT("arrow1-down");

		const TinyString TinyVisualPropertyConst::ARROW2NORMAL = TEXT("arrow2-normal");
		const TinyString TinyVisualPropertyConst::ARROW2HIGHLIGHT = TEXT("arrow2-highlight");
		const TinyString TinyVisualPropertyConst::ARROW2DOWN = TEXT("arrow2-down");

		const TinyString TinyVisualPropertyConst::SCROLLBARGROOVE = TEXT("scrollbar-groove");
		const TinyString TinyVisualPropertyConst::SCROLLBARNORMAL = TEXT("scrollbar-normal");
		const TinyString TinyVisualPropertyConst::SCROLLBARHIGHLIGHT = TEXT("scrollbar-highlight");

		const TinyString TinyVisualPropertyConst::SHADOWIMAGE = TEXT("shadow-image");
		const TinyString TinyVisualPropertyConst::SHADOWBOX = TEXT("shadow-box");

		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualCursor::AUTO = TEXT("auto");
		const TinyString TinyVisualCursor::HAND = TEXT("hand");
		const TinyString TinyVisualCursor::TEXT = TEXT("text");
		const TinyString TinyVisualCursor::MOVE = TEXT("move");
		const TinyString TinyVisualCursor::POINTER = TEXT("pointer");
		const TinyString TinyVisualCursor::WAIT = TEXT("wait");
		const TinyString TinyVisualCursor::HELP = TEXT("help");;
		const TinyString TinyVisualCursor::ERESIZE = TEXT("e-resize");;
		const TinyString TinyVisualCursor::NERESIZE = TEXT("ne-resize");
		const TinyString TinyVisualCursor::NWRESIZE = TEXT("nw-resize");
		const TinyString TinyVisualCursor::NRESIZE = TEXT("n-resize");
		const TinyString TinyVisualCursor::SERESIZE = TEXT("se-resize");
		const TinyString TinyVisualCursor::SWRESIZE = TEXT("sw-resize");
		const TinyString TinyVisualCursor::SRESIZE = TEXT("s-resize");
		const TinyString TinyVisualCursor::WRESIZE = TEXT("w-resize");
		LPSTR CursorFromString(TinyString& str)
		{
			if (str == TinyVisualCursor::HAND)
			{
				return IDC_HAND;
			}
			if (str == TinyVisualCursor::WAIT)
			{
				return IDC_WAIT;
			}
			if (str == TinyVisualCursor::HELP)
			{
				return IDC_HELP;
			}
			if (str == TinyVisualCursor::MOVE)
			{
				return IDC_CROSS;
			}
			if (str == TinyVisualCursor::ERESIZE)
			{
				return IDC_SIZEWE;
			}
			if (str == TinyVisualCursor::WRESIZE)
			{
				return IDC_SIZEWE;
			}
			if (str == TinyVisualCursor::SRESIZE)
			{
				return IDC_SIZENS;
			}
			if (str == TinyVisualCursor::NRESIZE)
			{
				return IDC_SIZENS;
			}
			if (str == TinyVisualCursor::NERESIZE)
			{
				return IDC_SIZENESW;
			}
			if (str == TinyVisualCursor::NWRESIZE)
			{
				return IDC_SIZENWSE;
			}
			if (str == TinyVisualCursor::SERESIZE)
			{
				return IDC_SIZENWSE;
			}
			if (str == TinyVisualCursor::SWRESIZE)
			{
				return IDC_SIZENESW;
			}
			return IDC_ARROW;
		}
		//////////////////////////////////////////////////////////////////////////
		const TinyString TinyVisualTag::WINDOW = TEXT("window");
		const TinyString TinyVisualTag::CONTEXT = TEXT("context");
		const TinyString TinyVisualTag::IMAGE = TEXT("image");
		const TinyString TinyVisualTag::ADD = TEXT("add");
		const TinyString TinyVisualTag::SYSCAPTION = TEXT("syscaption");
		const TinyString TinyVisualTag::BUTTON = TEXT("button");
		const TinyString TinyVisualTag::PANEL = TEXT("panel");
		const TinyString TinyVisualTag::STACKPANEL = TEXT("stackpanel");
		const TinyString TinyVisualTag::LIST = TEXT("list");
		const TinyString TinyVisualTag::LISTITEM = TEXT("listitem");
		const TinyString TinyVisualTag::LABEL = TEXT("label");
		const TinyString TinyVisualTag::ANIMATION = TEXT("animation");
		const TinyString TinyVisualTag::TAB = TEXT("tab");
		const TinyString TinyVisualTag::HBOXLAYOUT = TEXT("hboxlayout");
		const TinyString TinyVisualTag::VBOXLAYOUT = TEXT("vboxlayout");
		const TinyString TinyVisualTag::LISTBOX = TEXT("listbox");
		const TinyString TinyVisualTag::TOOLBAR = TEXT("toolbar");
		const TinyString TinyVisualTag::TREEVIEW = TEXT("treeview");
		const TinyString TinyVisualTag::HSCROLLBAR = TEXT("hscrollbar");
		const TinyString TinyVisualTag::VSCROLLBAR = TEXT("vscrollbar");
		const TinyString TinyVisualTag::COMBOBOX = TEXT("combobox");
		const TinyString TinyVisualTag::COMBOBOXITEM = TEXT("comboboxitem");
		const TinyString TinyVisualTag::OPTION = TEXT("option");
		const TinyString TinyVisualTag::TEXTBOX = TEXT("textbox");
		const TinyString TinyVisualTag::STRETCH = TEXT("stretch");
		const TinyString TinyVisualTag::NATIVE = TEXT("native");
		const TinyString TinyVisualTag::MENUITEM = TEXT("menuitem");
		//////////////////////////////////////////////////////////////////////////
	}
}
