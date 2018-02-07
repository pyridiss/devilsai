
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

#include <cmath>

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/filesystem.h"

#include "imageManager/imageManager.h"
#include "musicManager/musicManager.h"

#include "Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "gamedata.h"

using namespace tinyxml2;


/** FONCTIONS DE LA CLASSE Carte **/

Carte::Carte()
  : elements(),
    collidingItems(),
    onScreenItems(),
    places(),
    paths(),
    dataFiles(),
    commonDataLoaded(false),
    _stopManagement(false),
    _clock()
{
}

Carte::~Carte()
{
	for (auto& element : elements)
		delete element;

    for (auto& tmp : places)
        delete tmp.first;

    elements.clear();
    collidingItems.clear();
}

void Carte::insertItem(Element_Carte *elem)
{
    if (elem == nullptr) return;

    elements.push_back(elem);
    onScreenItems.push_back(elem);

    if (!elem->ignoreCollision)
        collidingItems.push_back(elem);
}

void Carte::removeItem(Element_Carte *elem)
{
    if (elem == nullptr) return;

    elements.remove(elem);

    if (!elem->ignoreCollision)
    {
        for (auto i = collidingItems.begin() ; i != collidingItems.end() ; ++i)
        {
            if (*i == elem)
            {
                collidingItems.erase(i);
                break;
            }
        }
    }
}

Individu* Carte::AjouterElement_Commun(string Type, string liste, int x, int y)
{
    Individu *ind = new Individu;

	ind->Liste = liste;
	ind->Type = Type;

    ind->setSpecies(gamedata::species(Type));

    if (ind->species() == nullptr)
	{
		delete ind;
		return NULL;
	}

    ind->species()->Copie_Element(ind);
    ind->move(x, y);

    insertItem(ind);
	return ind;
}

CheckPoint* Carte::addCheckPoint(string liste, int x, int y)
{
    CheckPoint *ind = new CheckPoint;

	ind->Liste = liste;

    ind->move(x, y);
    ind->size.setOrigin(&ind->position());

    insertItem(ind);
	return ind;
}

Coffre* Carte::AjouterCoffre(string liste, int x, int y)
{
	Coffre *ind = new Coffre;

	ind->Liste = liste;

    ind->move(x, y);
    ind->size.setOrigin(&ind->position());

    insertItem(ind);
	return ind;
}

void Carte::SupprimerElement(Element_Carte* elem)
{
    removeItem(elem);

    for (auto& item : elements)
        item->otherItemDeleted(elem);

    onScreenItems.remove(elem);

	delete elem;
}

void Carte::SupprimerListe(string num)
{
	for (auto tmp = elements.begin() ; tmp != elements.end() ; ++tmp)
	{
		if ((*tmp)->Liste == num)
		{
			SupprimerElement(*tmp);
			tmp = elements.begin();
		}
	}
}

void Carte::stopManagement()
{
    _stopManagement = true;
}

pair<Element_Carte*, int> Carte::findFirstCollidingItem(Individu* individual, tools::math::Shape& shape, bool apply)
{
    for (auto& item : collidingItems)
    {
        if (item == individual) continue;
        if (tools::math::intersection(shape, item->size))
        {
            int result = item->Collision(individual, apply);
            if (result != COLL_OK)
                return pair<Element_Carte*, int>(item, result);
        }
    }

    return pair<Element_Carte*, int>(nullptr, 0);
}

vector<pair<Element_Carte*, int>> Carte::findAllCollidingItems(Individu* individual, tools::math::Shape& shape, bool apply)
{
    vector<pair<Element_Carte*, int>> v;

    for (auto& item : collidingItems)
    {
        if (item == individual) continue;
        if (tools::math::intersection(shape, item->size))
        {
            int result = item->Collision(individual, apply);
            if (result != COLL_OK)
                v.push_back(pair<Element_Carte*, int>(item, result));
        }
    }

    return v;
}

bool comparisonBetweenElements(Element_Carte* a, Element_Carte* b)
{
	/* Classement des Élements selon leur TypeClassement
	 * - CLASSEMENT_SOL
	 * - CLASSEMENT_CADAVRE
	 * - CLASSEMENT_NORMAL & CLASSEMENT_HAUT
	 * - CLASSEMENT_NUAGE
	*/
	int aClass = a->TypeClassement;	if (aClass == CLASSEMENT_HAUT) aClass = CLASSEMENT_NORMAL;
	int bClass = b->TypeClassement;	if (bClass == CLASSEMENT_HAUT) bClass = CLASSEMENT_NORMAL;

	if (aClass < bClass) return true;
	if (aClass > bClass) return false;

	if (a->position().y >= b->position().y) return false;

	return true;
}

void Carte::GestionElements(const View& worldView)
{
	Element_Carte* ASupprimer = NULL;

    int max = worldView.getSize().x + 1500;
    _stopManagement = false;

	for (auto& tmp : elements)
	{
        if (abs((int)(tmp->position().x - worldView.getCenter().x)) <= max &&
            abs((int)(tmp->position().y - worldView.getCenter().y)) <= max)
        {
            if (tmp->Gestion() == ETAT_MORT)
            {
                ASupprimer = tmp;
            }
		}
        if (_stopManagement) break;
	}

    if (_clock.getElapsedTime().asMilliseconds() > 200)
    {
        int onScreenWidth = worldView.getSize().x + 300;
        onScreenItems.clear();
        for (auto& tmp : elements)
        {
            if (abs((int)(tmp->position().x - worldView.getCenter().x)) <= onScreenWidth &&
                abs((int)(tmp->position().y - worldView.getCenter().y)) <= onScreenWidth)
            {
                onScreenItems.push_back(tmp);
            }
        }

        onScreenItems.sort(comparisonBetweenElements);

        _clock.restart();
    }

	if (ASupprimer != NULL) SupprimerElement(ASupprimer);
}

void Carte::display(RenderTarget& target)
{
    for (auto& tmp : onScreenItems)
    {
        tmp->Disp(target);
    }
}

void Carte::displayBackground(RenderTarget& target)
{
	if (backgroundImage.empty()) return;

    Vector2f origin = target.getView().getCenter();
    origin.x -= target.getSize().x / 2;
    origin.y -= target.getSize().y / 2;

    for (int a = -1 ; a < (int)(target.getSize().x/135 + 2) ; ++a)
    {
        for (int b = -1 ; b < (int)(target.getSize().y/144 + 2) ; ++b)
        {
            imageManager::display(target, "paysage", backgroundImage, origin.x + 135*a - (int)(origin.x)%135, origin.y + 144*b - (int)(origin.y)%144);
        }
    }
}

Element_Carte* Carte::createNewItem(tinyxml2::XMLElement* item)
{
    Element_Carte* newItem = nullptr;

    string itemName = item->Name();
    if (itemName == "inertItem")
        newItem = new Paysage;
    else if (itemName == "individual")
        newItem = new Individu;
    else if (itemName == "storageBox")
        newItem = new Coffre;
    else if (itemName == "trigger")
        newItem = new Trigger;

    if (newItem == nullptr)
    {
        tools::debug::error("Unable to load item.", "files", __FILENAME__, __LINE__);
        return nullptr;
    }

    XMLHandle hdl3(item);
    newItem->loadFromXML(hdl3);

    return newItem;
}

void Carte::loadFromFile(string path, string tag)
{
    XMLDocument file;
    file.LoadFile(path.c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "properties")
        {
            if (elem->Attribute("backgroundImage"))
            {
                backgroundImage = elem->Attribute("backgroundImage");
                imageManager::addImage("paysage", backgroundImage, backgroundImage);
            }
        }
        if (elemName == "loadDataFile" && !commonDataLoaded)
        {
            string dataFile = elem->Attribute("file");
            gamedata::loadFromXML(tools::filesystem::dataDirectory(), dataFile);
            dataFiles.insert(std::move(dataFile));
        }
        if (elemName == "place" && !commonDataLoaded)
        {
            Paysage *p = new Paysage;
            string music;
            if (elem->Attribute("name")) p->Type = elem->Attribute("name");
            if (elem->Attribute("music"))
            {
                music = elem->Attribute("music");
                musicManager::addMusic(music);
            }

            XMLHandle hdl2(elem);
            XMLElement *elem2 = hdl2.FirstChildElement().ToElement();
            while (elem2)
            {
                string elem2Name = elem2->Name();
                if (elem2Name == "properties")
                {
                    elem2->QueryAttribute("diplomacy", &p->Diplomatie);
                }
                if (elem2Name == "shape")
                {
                    p->size.loadFromXML(elem2);
                }

                elem2 = elem2->NextSiblingElement();
            }
            places.push_back(pair<Element_Carte*, string>(p, music));
        }
        if (elemName == "path" && !commonDataLoaded)
        {
            pair<tools::math::Vector2d, tools::math::Shape> path;

            elem->QueryAttribute("x", &path.first.x);
            elem->QueryAttribute("y", &path.first.y);

            XMLHandle hdl2(elem);
            XMLElement *elem2 = hdl2.FirstChildElement().ToElement();
            while (elem2)
            {
                string elem2Name = elem2->Name();

                if (elem2Name == "shape")
                {
                    path.second.loadFromXML(elem2);
                    path.second.setOrigin(&path.first);
                }
                if (elem2Name == "inertItem")
                {
                    double repetitionStep = 1;
                    elem2->QueryAttribute("repetition-step", &repetitionStep);

                    if (path.second.points.size() < 2)
                    {
                        tools::debug::warning("Cannot add items along the path.", "files", __FILENAME__, __LINE__);
                        break;
                    }

                    double createdItems = 0;
                    tools::math::Vector2d currentVector = path.second.points[1] - path.second.points[0];
                    double currentAngle = tools::math::angle(currentVector.x, currentVector.y);
                    double lengthOnCurrentVector = -repetitionStep;
                    unsigned int index = 1;

                    while (createdItems * repetitionStep < path.second.length())
                    {
                        Element_Carte* newItem = createNewItem(elem2);

                        if (newItem == nullptr)
                        {
                            tools::debug::error("Error while creating a path.", "files", __FILENAME__, __LINE__);
                            break;
                        }

                        newItem->move(-newItem->position().x, -newItem->position().y);

                        if (currentVector.length() > lengthOnCurrentVector + repetitionStep)
                        {
                            newItem->move(path.second.points[index-1].x, path.second.points[index-1].y);
                            newItem->move(lengthOnCurrentVector * cos(currentAngle), lengthOnCurrentVector * sin(currentAngle));
                            newItem->move(repetitionStep * cos(currentAngle), repetitionStep * sin(currentAngle));
                            lengthOnCurrentVector += repetitionStep;
                        }
                        else
                        {
                            lengthOnCurrentVector += repetitionStep - currentVector.length();
                            ++index;
                            if (index >= path.second.points.size()) break;
                            currentVector = path.second.points[index] - path.second.points[index-1];

                            while (currentVector.length() <= lengthOnCurrentVector)
                            {
                                lengthOnCurrentVector -= currentVector.length();
                                ++index;
                                if (index >= path.second.points.size()) break;
                                currentVector = path.second.points[index] - path.second.points[index-1];
                            }

                            if (index >= path.second.points.size()) break;

                            currentAngle = tools::math::angle(currentVector.x, currentVector.y);
                            newItem->move(path.second.points[index-1].x, path.second.points[index-1].y);
                            newItem->move(lengthOnCurrentVector * cos(currentAngle), lengthOnCurrentVector * sin(currentAngle));
                        }

                        insertItem(newItem);

                        ++createdItems;
                    }
                }

                elem2 = elem2->NextSiblingElement();
            }

            paths.push_back(std::move(path));
        }
        else if (elemName == "items")
        {
            string currentTag = "ALL";
            bool Immuable = false;
            bool ignoreCollision = false;

            if (elem->Attribute("tag")) currentTag = elem->Attribute("tag");
            elem->QueryAttribute("immuable", &Immuable);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);

            if (currentTag == tag || tag == "ALL")
            {
                XMLHandle hdl2(elem);
                XMLElement *item = hdl2.FirstChildElement().ToElement();
                while (item)
                {
                    Element_Carte* newItem = createNewItem(item);

                    if (newItem != nullptr)
                    {
                        if (tag != "ALL") newItem->Liste = tag;
                        if (ignoreCollision) newItem->ignoreCollision = true;

                        insertItem(newItem);

                        if (Immuable) newItem->TypeClassement = CLASSEMENT_CADAVRE;
                    }

                    item = item->NextSiblingElement();
                }
            }
        }
        else if (elemName == "randomZone")
        {
            string currentTag = "ALL";
            bool Immuable = false;
            bool ignoreCollision = false;

            if (elem->Attribute("tag")) currentTag = elem->Attribute("tag");
            elem->QueryAttribute("immuable", &Immuable);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);

            if (currentTag != tag && tag != "ALL")
            {
                elem = elem->NextSiblingElement();
                continue;
            }

            tools::math::Shape* zone = nullptr;
            tools::math::Vector2d zoneOrigin(0, 0);
            bool customZone = false;
            XMLHandle hdl2(elem);
            XMLElement *item = hdl2.FirstChildElement().ToElement();

            //We create a list in which all genereated items are stored. This way, new items will not collide
            //one another. If this behavior is not wanted, the shape must be forced to none in the XML file.
            list<Element_Carte*> itemsList;

            //The zone can be defined from a place in the current world.
            if (elem->Attribute("place"))
            {
                string placeName = elem->Attribute("place");
                for (auto & p : places)
                {
                    if (p.first->Type == placeName) zone = &(p.first->size);
                }
            }
            else if (elem->Attribute("customZone"))
            {
                if (strcmp(item->Name(), "shape") == 0)
                {
                    zone = new tools::math::Shape();
                    customZone = true;
                    zone->loadFromXML(item);
                    zone->setOrigin(&zoneOrigin);
                }
                item = item->NextSiblingElement();
            }

            if (zone == nullptr)
            {
                tools::debug::error("A randomZone has been defined without a valid zone", "files", __FILENAME__, __LINE__);
                elem = elem->NextSiblingElement();
                continue;
            }

            pair<tools::math::Vector2d, tools::math::Vector2d> box = zone->box;

            while (item)
            {
                double quantity = 0;
                if (item->Attribute("density"))
                {
                    item->QueryAttribute("density", &quantity);
                    quantity *= zone->area() / 1000000.0;
                }
                if (item->Attribute("quantity"))
                {
                    item->QueryAttribute("quantity", &quantity);
                }

                int counter = 0;
                //'fake' will be used to test collisions with other items
                Individu fake;

                while (counter < quantity)
                {
                    Element_Carte* newItem = createNewItem(item);

                    if (newItem == nullptr)
                    {
                        tools::debug::error("Error while creating a randomZone.", "files", __FILENAME__, __LINE__);
                        break;
                    }

                    fake.size = newItem->size;
                    fake.size.setOrigin(&(newItem->position()));

                    int debugCounter = 0;
                    do
                    {
                        ++debugCounter;

                        //Set a new random position
                        newItem->move(-newItem->position().x, -newItem->position().y);
                        double x = box.first.x + (box.second.x - box.first.x)/10000.0 * (double)(rand()%10000);
                        double y = box.first.y + (box.second.y - box.first.y)/10000.0 * (double)(rand()%10000);
                        newItem->move(x, y);

                        //1. The item must collide with the zone
                        if (!tools::math::intersection(newItem->size, *zone)) continue;

                        //2. The item must not collide with paths
                        bool c = false;
                        for (auto& p : paths)
                        {
                            if (tools::math::intersection(newItem->size, p.second))
                                c = true;
                        }
                        if (c) continue;

                        //3. The item must not collide with other items already added
                        for (auto& i : itemsList)
                        {
                            if (tools::math::intersection(newItem->size, i->size))
                                c = true;
                        }
                        if (c) continue;

                        //4. The item must not collide with anything else
                        if (findFirstCollidingItem(&fake, newItem->size, false).first == nullptr)
                            break;
                    }
                    while (debugCounter < 100);

                    if (debugCounter == 100 && item->Attribute("quantity"))
                        tools::debug::error("Error while creating randomZone: no place left for " + newItem->Type, "files", __FILENAME__, __LINE__);

                    if (debugCounter < 100)
                    {
                        if (tag != "ALL") newItem->Liste = tag;
                        if (Immuable) newItem->TypeClassement = CLASSEMENT_CADAVRE;
                        if (ignoreCollision) newItem->ignoreCollision = true;
                        itemsList.push_back(newItem);
                    }
                    ++counter;
                }

                item = item->NextSiblingElement();
            }

            if (customZone && zone != nullptr)
                delete zone;

            for (auto& i : itemsList)
                insertItem(i);
        }
        elem = elem->NextSiblingElement();
    }

    commonDataLoaded = true;
}

void Carte::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* properties = doc.NewElement("properties");
    if (!backgroundImage.empty())
        properties->SetAttribute("backgroundImage", backgroundImage.c_str());
    root->InsertEndChild(properties);

    for (auto& tmp : dataFiles)
    {
        XMLElement* dataFile = doc.NewElement("loadDataFile");
        dataFile->SetAttribute("file", tmp.c_str());
        root->InsertEndChild(dataFile);
    }

    for (auto& tmp : places)
    {
        XMLElement* place = doc.NewElement("place");
        place->SetAttribute("name", tmp.first->Type.c_str());
        place->SetAttribute("music", tmp.second.c_str());

        XMLElement* placeProperties = doc.NewElement("properties");
        placeProperties->SetAttribute("diplomacy", tmp.first->Diplomatie);
        place->InsertEndChild(placeProperties);

        XMLElement* placeShape = doc.NewElement("shape");
        place->InsertEndChild(placeShape);
        XMLHandle shapeHandle(placeShape);
        tmp.first->size.saveToXML(doc, shapeHandle);

        root->InsertEndChild(place);
    }

    for (auto& tmp : paths)
    {
        XMLElement* path = doc.NewElement("path");
        path->SetAttribute("x", tmp.first.x);
        path->SetAttribute("y", tmp.first.y);

        XMLElement* pathShape = doc.NewElement("shape");
        path->InsertEndChild(pathShape);
        XMLHandle shapeHandle(pathShape);
        tmp.second.saveToXML(doc, shapeHandle);

        root->InsertEndChild(path);
    }

    XMLElement* _items = doc.NewElement("items");
    root->InsertEndChild(_items);
    XMLHandle itemsHandle(_items);

    for (auto& tmp : elements)
    {
        if (tmp != gamedata::player())
            tmp->saveToXML(doc, itemsHandle);
    }
}
