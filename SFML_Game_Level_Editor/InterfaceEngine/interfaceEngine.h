#pragma once

#include "interfaceEvent.h"
#include "interfaceAbstractBasicClass.h"
#include "interfaceObject.h"
#include "interfaceGroup.h"
#include "interfaceRenderRect.h"
#include "interfaceRectShape.h"
#include "interfaceDesktop.h"
#include "interfaceButton.h"
#include "interfaceText.h"
#include "interfaceWindow.h"
#include "interfaceDisplay.h"
#include "interfaceScrollbar.h"

namespace edt {

#define vec std::vector

	static const struct sObjectsJsonIds {
		static const unsigned int tDesktop		= 0;
		static const unsigned int tWindow		= 1;
		static const unsigned int tButton		= 2;
		static const unsigned int tText			= 3;
		static const unsigned int tRectShape	= 4;
		static const unsigned int tDisplay		= 5;
		static const unsigned int tScrollbar	= 6;
	} objects_json_ids;

}