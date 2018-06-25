#pragma once
#include "../Common/UserInterface.h"


#define HEADERSIG  0xA0B1C2D3;
string GameDirectoryPath = "D:\\Users\\Tim\\Documents\\era\\";

struct gui_file_header
{
	int header;
	int num_elements;
};

struct gui_element_header
{
	int header;
	int uitype;
};



int SaveGUIToFile(UserInterface* ui)
{
	gui_element_header header;
	gui_file_header fileHeader;
	fileHeader.header = HEADERSIG;
	header.header = HEADERSIG;
	std::ofstream outGUIFile((GameDirectoryPath + "ForgeGUI").data(), std::ios::out, std::ios::binary);
	int numElements = ui->GetNumberOfElements();

	if (!outGUIFile.is_open())
		return 0;

	fileHeader.num_elements = numElements;
	outGUIFile.write((char*)&fileHeader, sizeof(gui_file_header));

	for (int i = 0; i < numElements; i++)
	{
		UIElement* elm = ui->GetElement(i);

		switch (elm->GetType())
		{

		case UITYPE_BUTTON:
		{
			header.uitype = UITYPE_BUTTON;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIButton));
		}break;

		case UITYPE_TEXTFIELD:
		{
			header.uitype = UITYPE_TEXTFIELD;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UITextField));
		}break;

		case UITYPE_SELECTIONBOX:
		{
			header.uitype = UITYPE_SELECTIONBOX;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UISelectionBox));
		}break;

		case UITYPE_CHECKBOX:
		{
			header.uitype = UITYPE_CHECKBOX;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UICheckBox));
		}break;

		case UITYPE_RADIO:
		{
			header.uitype = UITYPE_RADIO;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIRadioGroup));
		}break;


		case UITYPE_DROPCONTAINER:
		{
			header.uitype = UITYPE_DROPCONTAINER;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIDropContainer));
		}break;

		case UITYPE_LIST:
		{
			header.uitype = UITYPE_LIST;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIList));
		}break;

		case UITYPE_REGION:
		{
			header.uitype = UITYPE_REGION;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIRegion));
		}break;

		case UITYPE_DROPDOWNMENU:
		{
			header.uitype = UITYPE_DROPDOWNMENU;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIDropdownMenu));
		}break;

		case UITYPE_WINDOW:
		{
			header.uitype = UITYPE_WINDOW;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIWindow));
		}break;

		case UITYPE_TEXT:
		{
			header.uitype = UITYPE_TEXT;
			outGUIFile.write((char*)&header, sizeof(gui_element_header));
			outGUIFile.write((char*)elm, sizeof(UIText));
		}	break;



		default: break;
		}

	}
	outGUIFile.close();

	return 1;
}



int LoadGUIFromFile(string filename, UserInterface* ui)
{
	std::ifstream inGUIFile((GameDirectoryPath + "ForgeGUI").data(), std::ios::in, std::ios::binary);
	if (!inGUIFile.is_open())
		return 0;

	gui_file_header fileHeader;
	//read in the file header
	inGUIFile.read((char*)&fileHeader, sizeof(gui_file_header));

	//read in all the elements
	gui_element_header elmHeader;
	for (int i = 0; i < fileHeader.num_elements; i++)
	{
		inGUIFile.read((char*)&elmHeader, sizeof(gui_element_header));

		switch (elmHeader.uitype)
		{

		case UITYPE_BUTTON:
		{
			UIButton* btn = new UIButton(0, 0);
			inGUIFile.read((char*)btn, sizeof(UIButton));
			ui->createButton(NULL, btn->GetPosition().x, btn->GetPosition().y, btn->GetText(), btn->GetRECT().getWidth(), btn->GetRECT().getHeight());
			delete btn;

		}break;

		case UITYPE_TEXTFIELD:
		{
			UITextField* txtField = new UITextField();
			inGUIFile.read((char*)txtField, sizeof(UITextField));
			ui->createTextField(NULL, txtField->GetPosition().x, txtField->GetPosition().y, txtField->GetCellCount());
			delete txtField;

		}break;

		case UITYPE_SELECTIONBOX:
		{
			UISelectionBox* selBox = new UISelectionBox();
			inGUIFile.read((char*)selBox, sizeof(UISelectionBox));
			ui->createSelectionBox(selBox->GetPosition().x, selBox->GetPosition().y, selBox->GetSelectedTitle());
			delete selBox;

		}break;

		case UITYPE_CHECKBOX:
		{
			UICheckBox* checkBox = new UICheckBox();
			inGUIFile.read((char*)checkBox, sizeof(UICheckBox));
			ui->createCheckBox(checkBox->GetPosition().x, checkBox->GetPosition().y, checkBox->IsChecked());
			delete checkBox;

		}break;

		case UITYPE_RADIO:
		{
			UIRadioGroup* radio = new UIRadioGroup();
			inGUIFile.read((char*)radio, sizeof(UIRadioGroup));
			ui->createRadioGroup(radio->GetPosition().x, radio->GetPosition().y, radio->GetNumButtons());
			delete radio;

		}break;


		case UITYPE_DROPCONTAINER:
		{
			UIDropContainer* dropContainer = new UIDropContainer();
			inGUIFile.read((char*)dropContainer, sizeof(UIDropContainer));
			ui->createDropContainer(dropContainer->GetPosition().x, dropContainer->GetPosition().y);
			delete dropContainer;

		}break;

		case UITYPE_LIST:
		{
			UIList* uilist = new UIList();
			inGUIFile.read((char*)uilist, sizeof(UIList));
			ui->createList(uilist->GetPosition().x, uilist->GetPosition().y, uilist->GetRECT().getWidth());
			delete uilist;

		}break;

		case UITYPE_REGION:
		{
			UIRegion* uiregion = new UIRegion();
			inGUIFile.read((char*)uiregion, sizeof(UIRegion));
			ui->createRegion(uiregion->GetPosition().x, uiregion->GetPosition().y, uiregion->GetName(), uiregion->GetRECT().getWidth(), uiregion->GetRECT().getHeight());
			delete uiregion;

		}break;

		case UITYPE_DROPDOWNMENU:
		{
			UIDropdownMenu* uiDropMenu = new UIDropdownMenu();
			inGUIFile.read((char*)uiDropMenu, sizeof(UIDropdownMenu));
			ui->createDropdownMenu(uiDropMenu->GetPosition().x, uiDropMenu->GetPosition().y, uiDropMenu->GetTitle());
			delete uiDropMenu;

		}break;

		case UITYPE_WINDOW:
		{
			UIWindow* uiWindow = new UIWindow();
			inGUIFile.read((char*)uiWindow, sizeof(UIWindow));
			ui->createWindow(uiWindow->GetPosition().x, uiWindow->GetPosition().y, uiWindow->GetRECT().getWidth(), uiWindow->GetRECT().getHeight(), uiWindow->GetTitle(), uiWindow->GetBackgroundColor());
			delete uiWindow;

		}break;

		case UITYPE_TEXT:
		{
			UIText* uiText = new UIText();
			inGUIFile.read((char*)uiText, sizeof(UIText));
			ui->createText(uiText->GetPosition().x, uiText->GetPosition().y, uiText->GetRECT().getHeight() - 2, uiText->GetText());
			delete uiText;
		}	break;



		default: break;
		}

	}


}
