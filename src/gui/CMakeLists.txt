add_library(gui button.cpp dropDownList.cpp inputField.cpp style.cpp scrollingList.cpp textWidget.cpp widget.cpp window.cpp)

target_link_libraries(gui ${SFML_LIBRARIES} ${TinyXML2_LIBRARIES})
