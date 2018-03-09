/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <fs/DirList.h>
#include <fs/FSUtils.h>
#include <utils/utils.h>

#include "HomebrewLaunchWindow.h"
#include "HomebrewLoader.h"
#include "common/common.h"
#include "custom/utils/HomebrewXML.h"
#include "Application.h"

HomebrewLaunchWindow::HomebrewLaunchWindow(const std::string & launchPath, GuiImageData * iconImgData)
    : GuiFrame(0, 0)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , backgroundImgData(Resources::GetImageData("launchMenuBox.png"))
    , backgroundImg(backgroundImgData)
    , buttonImgData(Resources::GetImageData("button.png"))
    , iconImage(iconImgData)
    , titleText((char*)NULL, 42, glm::vec4(1.0f))
    , versionText("Version:", 20, glm::vec4(1.0f))
    , versionValueText((char*)NULL, 20, glm::vec4(1.0f))
    , authorText("Author:", 20, glm::vec4(1.0f))
    , authorValueText((char*)NULL, 20, glm::vec4(1.0f))
    , loadBtnLabel("Load", 24, glm::vec4(1.0f))
    , loadImg(buttonImgData)
    , loadBtn(loadImg.getWidth(), loadImg.getHeight())
    , backBtnLabel("Back", 24, glm::vec4(1.0f))
    , backImg(buttonImgData)
    , backBtn(backImg.getWidth(), backImg.getHeight())
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , homebrewLaunchPath(launchPath)
    , buttonATrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_A, true)
    , buttonBTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_B, true)
{
    width = backgroundImg.getWidth();
    height = backgroundImg.getHeight();
    append(&backgroundImg);

    std::string homebrewPath = launchPath;
    size_t slashPos = homebrewPath.rfind('/');
    if(slashPos != std::string::npos)
        homebrewPath.erase(slashPos);

    HomebrewXML metaXml;
    bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());

    int yOffset = height * 0.5f - 75.0f;

    const char *cpName = xmlReadSuccess ? metaXml.GetName() : launchPath.c_str();
    if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
       cpName += strlen("sd:/wiiu/apps/");

    titleText.setText(cpName);
    titleText.setColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	titleText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    titleText.setPosition(-70, 200);
    append(&titleText);

    float scaleFactor = 1.0f;
    iconImage.setAlignment(ALIGN_LEFT | ALIGN_TOP);
    iconImage.setPosition(120, -100);
    iconImage.setSize(256, 256);
    append(&iconImage);

    yOffset -= 50;

    versionText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	versionText.setColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    versionText.setPosition(-118, 130);
    append(&versionText);

    versionValueText.setTextf("%s", xmlReadSuccess ? metaXml.GetVersion() : launchPath.c_str());
    versionValueText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	versionValueText.setColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    versionValueText.setPosition(-50, 130);
    append(&versionValueText);
    yOffset -= 30;

    authorText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	authorText.setColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    authorText.setPosition(-120, 157);
    append(&authorText);

    authorValueText.setTextf("%s", metaXml.GetCoder());
	authorValueText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	authorValueText.setColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    authorValueText.setPosition(-30, 157);
    append(&authorValueText);
    yOffset -= 50;

    scaleFactor = 1.0f;
    loadImg.setScale(scaleFactor);
    loadBtn.setSize(scaleFactor * loadImg.getWidth(), scaleFactor * loadImg.getHeight());
    loadBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    loadBtn.setPosition(547, -355);
	loadBtn.setTrigger(&buttonATrigger);
    loadBtn.setTrigger(&touchTrigger);
    loadBtn.setTrigger(&wpadTouchTrigger);
    loadBtn.clicked.connect(this, &HomebrewLaunchWindow::OnLoadButtonClick);
    append(&loadBtn);

    backImg.setScale(scaleFactor);
    backBtn.setSize(scaleFactor * backImg.getWidth(), scaleFactor * backImg.getHeight());
    backBtn.setImage(&backImg);
    backBtn.setLabel(&backBtnLabel);
    backBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    backBtn.setPosition(200, -310);
	backBtn.setTrigger(&buttonBTrigger);
	backBtn.setTrigger(&touchTrigger);
	backBtn.setTrigger(&wpadTouchTrigger);
    backBtn.setEffectGrow();
    backBtn.setSoundClick(buttonClickSound);
    backBtn.clicked.connect(this, &HomebrewLaunchWindow::OnBackButtonClick);
    append(&backBtn);
}

HomebrewLaunchWindow::~HomebrewLaunchWindow()
{
    Resources::RemoveSound(buttonClickSound);
    Resources::RemoveImageData(backgroundImgData);
    Resources::RemoveImageData(buttonImgData);
}

void HomebrewLaunchWindow::OnOpenEffectFinish(GuiElement *element)
{
    //! once the menu is open reset its state and allow it to be "clicked/hold"
    element->effectFinished.disconnect(this);
    element->clearState(GuiElement::STATE_DISABLED);
}

void HomebrewLaunchWindow::OnCloseEffectFinish(GuiElement *element)
{
    //! remove element from draw list and push to delete queue
    remove(element);
    AsyncDeleter::pushForDelete(element);

    backBtn.clearState(GuiElement::STATE_DISABLED);
    loadBtn.clearState(GuiElement::STATE_DISABLED);
}

void HomebrewLaunchWindow::OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result)
{
    element->setState(GuiElement::STATE_DISABLED);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewLaunchWindow::OnCloseEffectFinish);

    if(result > 0)
    {
        Application::instance()->quit(EXIT_SUCCESS);
    }
}


void HomebrewLaunchWindow::OnLoadButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    backBtn.setState(GuiElement::STATE_DISABLED);
    loadBtn.setState(GuiElement::STATE_DISABLED);

    HomebrewLoader * loader = HomebrewLoader::loadToMemoryAsync(homebrewLaunchPath);
    loader->setEffect(EFFECT_FADE, 15, 255);
    loader->effectFinished.connect(this, &HomebrewLaunchWindow::OnOpenEffectFinish);
    loader->asyncLoadFinished.connect(this, &HomebrewLaunchWindow::OnFileLoadFinish);
    append(loader);
}
