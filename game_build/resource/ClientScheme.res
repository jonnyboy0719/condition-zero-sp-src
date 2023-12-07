//
// TRACKER SCHEME RESOURCE FILE
//
// sections:
//		colors			- all the colors used by the scheme
//		basesettings	- contains settings for app to use to draw controls
//		fonts			- list of all the fonts used by app
//		borders			- description of all the borders
//
// notes:
// 		hit ctrl-alt-shift-R in the app to reload this file
//
Scheme
{
	//Name - currently overriden in code
	//{
	//	"Name"	"ClientScheme"
	//}

	//////////////////////// COLORS ///////////////////////////
	Colors
	{
		// base colors
		"BaseText"			"255 176 0 255"	// used in text windows, lists
		"BrightBaseText"	"255 176 0 255"	// brightest text
		"SelectedText"		"255 176 0 255"	// selected text
		"DimBaseText"		"255 176 0 255"	// dim base text
		"LabelDimText"		"255 176 0 164"	// used for info text
		"ControlText"		"255 176 0 255"	// used in all text controls
//		"BrightControlText"	"255 176 0 255"	// use for selected controls
		"BrightControlText"	"255 255 255 255"	// use for selected controls
		"DisabledText1"		"255 255 255 255"		// disabled text
		"DisabledText2"		"0 0 0 255"	// overlay color for disabled text (to give that inset look)
		"DimListText"		"188 112 0 255"	// offline friends, unsubscribed games, etc.

		// background colors
		"ControlBG"			"0 0 0 0"		// background color of controls
		"ControlDarkBG"		"0 0 0 128"		// darker background color; used for background of scrollbars
		"WindowBG"			"0 0 0 200"		// background color of text edit panes (chat, text entries, etc.)
		"SelectionBG"		"192 28 0 140"	// background color of any selected text or menu item
		"SelectionBG2"		"70 12 0 0"		// selection background in window w/o focus
		"ListBG"			"0 0 0 128"		// background of scoreboard
		"ViewportBG"		"0 0 0 200"

		// titlebar colors
		"TitleText"			"255 174 0 255"
		"TitleDimText"		"255 174 0 255"
		"TitleBG"			"255 255 0 0"
		"TitleDimBG"		"255 255 0 0"

		"ButtonBorderColor" "255 255 255 255"
		
		// slider tick colors
		"SliderTickColor"		"127 140 127 255"
		"SliderTrackColor"	"31 31 31 255"

		// border colors
		"BorderBright"		"188 112 0 128"	// the lit side of a control
		"BorderDark"		"188 112 0 128"	// the dark/unlit side of a control
		"BorderSelection"		"188 112 0 0"	// the additional border color for displaying the default/selected button

		"ButtonBorder"			"189 189 189 255"
		"ButtonBorderDark"		"189 189 189 255"
		"ButtonBorderArmed"		"255 242 0 255"
		"ButtonBorderArmedDark"		"255 242 0 255"
		"ButtonBorderDepressed"		"255 242 0 255"
		"ButtonBorderDepressedDark"	"255 242 0 255"

		"ButtonDepressedBorderBright"	"255 242 0 255"
		"ButtonDepressedBorderDark"	"255 242 0 255"


		"team0"			"125 165 210"
		"team1"			"200 90 70"
		"team2"			"225 205 45"
		"team3"			"145 215 140"
		"team4"			"255 170 0"

		"MapDescriptionText"	"255 176 0 255" // the text used in the map description window
	}

	///////////////////// BASE SETTINGS ////////////////////////
	// default settings for all panels
	// controls use these to determine their settings
	BaseSettings
	{
		"FgColor"			"ControlText"
		"BgColor"			"ControlBG"
		"LabelBgColor"		"ControlBG"
		"SubPanelBgColor"	"ControlBG"

		"DisabledFgColor1"		"DisabledText1" 
		"DisabledFgColor2"		"ControlBG"			// set this to the BgColor if you don't want it to draw

		"TitleBarFgColor"			"TitleText"
		"TitleBarDisabledFgColor"	"TitleDimText"
		"TitleBarBgColor"			"TitleBG"
		"TitleBarDisabledBgColor"	"TitleDimBG"

		"TitleBarIcon"				"resource/icon_steam"
		"TitleBarDisabledIcon"		"resource/icon_steam_disabled"

		"TitleButtonFgColor"			"BorderBright"
		"TitleButtonBgColor"			"ControlBG"
		"TitleButtonDisabledFgColor"	"TitleDimText"
		"TitleButtonDisabledBgColor"	"TitleDimBG"

		"TextCursorColor"			"BaseText"			// color of the blinking text cursor in text entries
		"URLTextColor"				"BrightBaseText"	// color that URL's show up in chat window

		Menu
		{
			"FgColor"			"DimBaseText"
			"BgColor"			"ControlBG"
			"ArmedFgColor"		"BrightBaseText"
			"ArmedBgColor"		"SelectionBG"
			"DividerColor"		"BorderDark"
			"TextInset"			"6"
		}

		MenuButton	  // the little arrow on the side of boxes that triggers drop down menus
		{
			"ButtonArrowColor"	"DimBaseText"		// color of arrows
		   	"ButtonBgColor"		"WindowBG"			// bg color of button. same as background color of text edit panes 
			"ArmedArrowColor"	"BrightBaseText"	// color of arrow when mouse is over button
			"ArmedBgColor"		"DimBaseText"		// bg color of button when mouse is over button
		}

		Slider
		{
			"SliderFgColor"		"ControlBG"			// handle with which the slider is grabbed
			"SliderBgColor"		"ControlDarkBG"		// area behind handle
		}

		ScrollBarSlider
		{
			"BgColor"					"ControlBG"		// this isn't really used
			"ScrollBarSliderFgColor"	"ControlBG"		// handle with which the slider is grabbed
			"ScrollBarSliderBgColor"	"ControlDarkBG"	// area behind handle
			"ButtonFgColor"				"DimBaseText"	// color of arrows
		}


		// text edit windows
		"WindowFgColor"				"BaseText"		// off-white
		"WindowBgColor"				"WindowBG"		// redundant. can we get rid of WindowBgColor and just use WindowBG?
		"WindowDisabledFgColor"		"DimBaseText"
		"WindowDisabledBgColor"		"ListBG"		// background of chat conversation
		"SelectionFgColor"			"SelectedText"	// fg color of selected text
		"SelectionBgColor"			"SelectionBG"
		"ListSelectionFgColor"		"SelectedText"
		"ListBgColor"				"ListBG"		// background of server browser control, etc
		"BuddyListBgColor"			"ListBG"		// background of buddy list pane
		
		// App-specific stuff
		"ChatBgColor"				"WindowBG"

		// status selection
		"StatusSelectFgColor"		"BrightBaseText"
		"StatusSelectFgColor2"		"BrightControlText"	// this is the color of the friends status

		// checkboxes
		"CheckButtonBorder1"   		"255 255 255 255"		// the left checkbutton border
		"CheckButtonBorder2"   		"255 255 255 255"		// the right checkbutton border
		"CheckButtonCheck"			"255 242 0 255"	// color of the check itself
		"CheckBgColor"				"0 0 0 0"

		// buttons (default fg/bg colors are used if these are not set)
//		"ButtonArmedFgColor"
		"ButtonArmedBgColor"		"SelectionBG"
//		"ButtonDepressedFgColor"	"BrightControlText"
//		"ButtonDepressedBgColor"

		// buddy buttons
		BuddyButton
		{
			"FgColor1"				"ControlText"
			"FgColor2"				"DimListText"
			"ArmedFgColor1"			"BrightBaseText"
			"ArmedFgColor2"			"BrightBaseText"
			"ArmedBgColor"			"SelectionBG"
		}

		Chat
		{
			"TextColor"				"BrightControlText"
			"SelfTextColor"			"BaseText"
			"SeperatorTextColor"	"DimBaseText"
		}

		WorldMap
		{ 
			"MissionButtonFgColor"		"255 255 255 255" // mission button current mission
			"MissionButtonBgColor"		"0 0 0 0"		// 
			"ButtonArmedFgColor"		"255 242 0 255"		 // **** mouse over mission button current mission
			"ButtonArmedBgColor"		"0 0 0 0" // 
			"ButtonDepressedFgColor"	"255 255 255 255" // mission button current mission
			"ButtonDepressedBgColor"	"0 0 0 0" // 

			"BgColor"					"0 0 0 255"
			"FgColor"					"0 0 0 0"
			"BgColorMission"			"0 0 0 255"
			"MissionTitleFgColor"		"24 214 59 255"		 // mission title color
			"MissionLabelFgColor"		"255 255 255 255"		
			"MSLabelFgColor"			"189 189 189 255"	
			"MissionLabelFgTextColor"	"189 189 189 255" // the word "MISSION:"

			"ResumeTextColor"			"255 255 255 255"		
			"ResumeDefaultFgColor"		"255 255 255 255"		
			"ResumeArmedFgColor"		"255 242 0 255"	
			"ResumeDepressedFgColor"	"255 242 0 255"
			"ResumeDisabledFgColor1"	"189 189 189 255"	
			"ResumeDisabledFgColor2"	"0 0 0 0"
			"ResumeDefaultBgColor"		"0 0 0 0"		// medium blue green
			"ResumeArmedBgColor"		"0 0 0 0"	// brighter blue green
			"ResumeDepressedBgColor"	"0 0 0 0"	// brighter blue green with alpha
		}

		Mission
		{
			"MissionTitleFgColor"		"24 214 59 255"		// name of the mission
			"MissionLabelFgColor"		"24 214 69 255"		// dark blue green
			"MSLabelFgColor"			"189 189 189 255"	// bright blue green
			"BgColorMission"			"0 0 0 255"
			"ButtonTextColor"			"255 255 255 255"		// dark blue green
			"ButtonDefaultFgColor"		"189 189 189 255"		// dark blue green
			"ButtonArmedFgColor"		"255 242 0 255"		// dark blue green
			"ButtonDepressedFgColor"	"255 242 0 255"
			"ButtonDefaultBgColor"		"0 0 0 0"		//  clear
			"ButtonArmedBgColor"		"0 0 0 0"	// brighter blue green
			"ButtonDepressedBgColor"	"0 0 0 0"	// brighter blue green with alpha
			"ButtonDisabledFgColor1"	"189 189 189 255"	
			"ButtonDisabledFgColor2"	"0 0 0 0"

			"MissionLabelFgTextColor"	"189 189 189 255" // the word "MISSION:"


		}

		"SectionTextColor"		"BrightControlText"	// text color for IN-GAME, ONLINE, OFFLINE sections of buddy list
		"SectionDividerColor"	"BorderDark"		// color of line that runs under section name in buddy list
	}

	//
	//////////////////////// FONTS /////////////////////////////
	//
	// describes all the fonts
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace
		"Default"
		{
			"1"
			{
				"name"		"Arial"
				"tall"		"12"
				"weight"	"800"
			}
		}
		"DefaultUnderline"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"500"
				"underline" "1"
			}
		}
		"DefaultSmall"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"11"
				"weight"	"800"
			}
		}
		"DefaultVerySmall"
		{
			"1"
			{
				"name"		"Verdana"
				"tall"		"10"
				"weight"	"0"
			}
		}

		// this is the symbol font
		"Marlett"
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"14"
				"weight"	"0"
				"symbol"	"1"
			}
		}

		// world map font
		"MissionFont" // mission buttons on the world map
		{
			"1"
			{
				"name"			"Arial"
				"tall"			"12"
				"weight"		"500"
			}
		}

		"MissionTitleFont" // MISSION: text
		{
			"1"
			{
				"name"			"Arial"
				"tall"			"15"
				"weight"		"800"
			}
		}

		"MissionLabelFont" // name of mission text
		{
			"1"
			{
				"dropshadow"	"0"
				"name"			"Verdana"
				"tall"			"15"
				"weight"		"800"
			}
		}

		"DataLabelFont" // mission detailed description text
		{
			"1"
			{
				"dropshadow"	"0"
				"name"			"Arial"
				"tall"			"12"
				"weight"		"500"
			}
		}

		"DataLabelBoldFont" // mission detailed description text
		{
			"1"
			{
				"dropshadow"	"0"
				"name"			"Arial"
				"tall"			"12"
				"weight"		"800"
			}
		}

	}

	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		BaseBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}
		
		TitleButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"4"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		TitleButtonDisabledBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BgColor"
					"offset" "1 0"
				}
			}
			Top
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 0"
				}
			}
		}

		TitleButtonDepressedBorder
		{
			"inset" "1 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonBorder
		{
			"inset" "1 0 0 0"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonDepressedBorder
		{
			"inset" "2 2 0 0"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}
		
		ButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ButtonBorderColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ButtonBorderColor"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ButtonBorderColor"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ButtonBorderColor"
					"offset" "0 0"
				}
			}
		}

		FrameBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		TabBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		TabActiveBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "6 2"
				}
			}
		}


		ToolTipBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		// this is the border used for default buttons (the button that gets pressed when you hit enter)
		ButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}
			Top
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}
			Right
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}
			Bottom
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderDark"
					"offset" "1 1"
				}
			}
		}

		ButtonDepressedBorder
		{
			"inset" "2 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ComboBoxBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		MenuBorder
		{
			"inset" "1 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}
		BrowserBorder
		{
			"inset" "0 0 0 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}


		// world map borders
		WorldMapMissionButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		WorldMapMissionButtonDepressedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		WorldMapMissionButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		// world map borders
		MSButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		MSButtonDepressedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		MSButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}
			
		HighlightButtonBorder
		{
			"inset" "0 0 0 0"
			Left
			{
				"1"
				{
					"color" "WorldMap/ResumeDefaultFgColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "WorldMap/ResumeDefaultFgColor"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "WorldMap/ResumeDefaultFgColor"
					"offset" "1 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "WorldMap/ResumeDefaultFgColor"
					"offset" "0 0"
				}
			}
		}


		HighlightButtonArmedBorder
		{
			"inset" "0 0 0 0"
			Left
			{
				"1"
				{
					"color" "WorldMap/ResumeArmedFgColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "WorldMap/ResumeArmedFgColor"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "WorldMap/ResumeArmedFgColor"
					"offset" "1 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "WorldMap/ResumeArmedFgColor"
					"offset" "0 0"
				}
			}
		}


		HighlightButtonDepressedBorder
		{
			"inset" "0 0 0 0"
			Left
			{
				"1"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "0 1"
				}
				"2"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "1 2"
				}
				"3"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "2 3"
				}
			}

			Right
			{
				"1"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "0 0"
				}
				"2"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "1 1"
				}
				"3"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "2 2"
				}
			}

			Top
			{
				"1"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "1 1"
				}
				"2"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "2 2"
				}
				"3"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "3 3"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "0 0"
				}
				"2"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "1 1"
				}
				"3"
				{
					"color" "WorldMap/ResumeDepressedFgColor"
					"offset" "2 2"
				}
			}
		}


	}
}