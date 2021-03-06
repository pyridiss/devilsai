
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <physfs.h>

#include <iostream>

#include "gui/style.h"
#include "tools/debug.h"
#include "tools/graphicBuffer.h"
#include "tools/timeManager.h"
#include "tools/math.h"
#include "tools/filesystem.h"

#include "Bibliotheque/Bibliotheque.h"

#include "Jeu.h"

#include "imageManager/imageManager.h"

#include "options.h"

#include "devilsai-resources/shaders.h"

RenderWindow App;

void createWindow()
{
    unsigned w = options::option<unsigned>(tools::hash("screen-width"));
    unsigned h = options::option<unsigned>(tools::hash("screen-height"));

    VideoMode vm = VideoMode(w, h, VideoMode::getDesktopMode().bitsPerPixel);

    if (options::option<bool>(tools::hash("screen-fullscreen")) && vm.isValid())
    {
        App.create(vm, "devilsai", Style::Fullscreen);
    }
    else
    {
        App.create(vm, "devilsai");
    }

    App.setFramerateLimit(60);
    App.setKeyRepeatEnabled(false);
    App.setVerticalSyncEnabled(true);

}

/** DÉBUT DU PROGRAMME **/
int main(int n, char *params[])
{
    tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath());

    tools::debug::openDebugFile("devilsai", VERSION);
    tools::debug::addDebugCategory("devilsai");
    tools::filesystem::setDataDirectory(INSTALL_DIR);
    tools::math::initLibrary();

    multimedia::addFont("liberation", "LiberationSans-Regular.ttf");
    multimedia::addFont("liberation-bold", "LiberationSans-Bold.ttf");
    multimedia::addFont("dayroman", "DayRoman.ttf");

    devilsai::initShaders();

	PHYSFS_init(params[0]);

    options::loadFromFile(tools::filesystem::dataDirectory() + "options.xml");
    options::loadFromFile(tools::filesystem::getSaveDirectoryPath() + "options.xml");
    options::loadFromCommandLine(n, params);

    tools::timeManager::setSpeed(60.0/1000.0);

    createWindow();

	srand(time(NULL));

    tools::graphicBuffer::initLibrary(&App);

    gui::parameterize(tools::hash("text-font"), "liberation"s);
    gui::parameterize(tools::hash("text-color"), Color::Black);
    gui::parameterize(tools::hash("text-size"), 10);
    gui::parameterize(tools::hash("text-disabled-shader"), "contrast"s);
    gui::parameterize("text-disabled-shader-instance"_hash, devilsai::newContrastShaderInstance(Glsl::Vec3(0.25, 0.25, 0.25)));
    gui::parameterize(tools::hash("text-mouseover-shader"), "contrast"s);
    gui::parameterize("text-mouseover-shader-instance"_hash, devilsai::newContrastShaderInstance(Glsl::Vec3(1.0, 0.0, 0.0)));
    gui::parameterize(tools::hash("background-disabled-shader"), "contrast"s);
    gui::parameterize("background-disabled-shader-instance"_hash, devilsai::newContrastShaderInstance(Glsl::Vec3(0.25, 0.25, 0.25)));
    gui::parameterize(tools::hash("background-mouseover-shader"), "contrast"s);
    gui::parameterize("background-mouseover-shader-instance"_hash, devilsai::newContrastShaderInstance(Glsl::Vec3(1.0, 1.0, 1.0)));
    gui::parameterize(tools::hash("button-text-font"), "liberation-bold"s);
    gui::parameterize(tools::hash("button-text-color"), Color(226, 8, 0, 255));
    gui::parameterize(tools::hash("button-text-size"), 12);
    gui::parameterize(tools::hash("list-text-font"), "liberation-bold"s);
    gui::parameterize(tools::hash("list-text-size"), 16);
    gui::parameterize(tools::hash("list-text-color"), Color::White);
    gui::parameterize(tools::hash("list-background-color"), Color::Black);
    gui::parameterize(tools::hash("list-highlight-color"), Color(100, 100, 100));
    gui::parameterize(tools::hash("list-selected-color"), Color::Red);
    gui::parameterize(tools::hash("input-text-font"), "liberation"s);
    gui::parameterize(tools::hash("input-text-color"), Color::Black);
    gui::parameterize(tools::hash("input-text-size"), 15);

    if (!gui::initLibrary())
        return EXIT_FAILURE;

	Load_Decorations();

    imageManager::addContainer(tools::hash("individuals"));
    imageManager::addContainer(tools::hash("movingObjects"));
    imageManager::addContainer(tools::hash("paysage"));
    imageManager::addContainer(tools::hash("skills"));
    imageManager::addContainer(tools::hash("objects"));
    imageManager::addContainer(tools::hash("objectsIcons"));
    imageManager::addContainer("gui"_hash);
    imageManager::addImage("gui"_hash, "healthgauge-life", "img/gui-healthgauge-life.png");
    imageManager::addImage("gui"_hash, "healthgauge-energy", "img/gui-healthgauge-energy.png");
    imageManager::addImage("gui"_hash, "healthgauge-healing", "img/gui-healthgauge-healing.png");

    mainLoop(App);
    Clean_Partie();

	PHYSFS_deinit();

    options::Save_Options();


    tools::debug::message("Closing devilsai debug file.", "devilsai", __FILENAME__, __LINE__);

	return EXIT_SUCCESS;
}
