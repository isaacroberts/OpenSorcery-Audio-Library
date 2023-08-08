//
//  PopupSelector.cpp
//  
//
//  Created by Isaac Roberts on 8/21/22.
//

// Created by Isaac Roberts


//#include "PopupSelector.h"

namespace juce {
	
	

PopupSelector::PopupSelector (const String& name)
	: Component (name),
	targetPopupBounds(0, 0, 200, 50)
{
	setRepaintsOnMouseActivity (true);
	lookAndFeelChanged();
	currentId.addListener (this);
}

PopupSelector::~PopupSelector()
{
	currentId.removeListener (this);
	hidePopup();
//	label.reset();
}

void PopupSelector::setTooltip (const String& newTooltip)
{
	SettableTooltipClient::setTooltip (newTooltip);
}

//==============================================================================
void PopupSelector::addItem (const String& newItemText, int newItemId)
{
	// you can't add empty strings to the list..
	jassert (newItemText.isNotEmpty());

	// IDs must be non-zero, as zero is used to indicate a lack of selection.
	jassert (newItemId != 0);

	// you shouldn't use duplicate item IDs!
	jassert (getItemForId (newItemId) == nullptr);

	if (newItemText.isNotEmpty() && newItemId != 0)
		currentMenu.addItem (newItemId, newItemText, true, false);
}

void PopupSelector::addItemList (const StringArray& itemsToAdd, int firstItemID)
{
	for (auto& i : itemsToAdd)
		currentMenu.addItem (firstItemID++, i);
}

void PopupSelector::addSeparator()
{
	currentMenu.addSeparator();
}

void PopupSelector::addSectionHeading (const String& headingName)
{
	// you can't add empty strings to the list..
	jassert (headingName.isNotEmpty());

	if (headingName.isNotEmpty())
		currentMenu.addSectionHeader (headingName);
}

void PopupSelector::setItemEnabled (int itemId, bool shouldBeEnabled)
{
	if (auto* item = getItemForId (itemId))
		item->isEnabled = shouldBeEnabled;
}

bool PopupSelector::isItemEnabled (int itemId) const noexcept
{
	if (auto* item = getItemForId (itemId))
		return item->isEnabled;

	return false;
}

void PopupSelector::changeItemText (int itemId, const String& newText)
{
	if (auto* item = getItemForId (itemId))
		item->text = newText;
	else
		jassertfalse;
}

void PopupSelector::clear (const NotificationType notification)
{
	currentMenu.clear();
	setSelectedItemIndex (-1, notification);
}

//==============================================================================
PopupMenu::Item* PopupSelector::getItemForId (int itemId) const noexcept
{
	if (itemId != 0)
	{
		for (PopupMenu::MenuItemIterator iterator (currentMenu, true); iterator.next();)
		{
			auto& item = iterator.getItem();

			if (item.itemID == itemId)
				return &item;
		}
	}

	return nullptr;
}

PopupMenu::Item* PopupSelector::getItemForIndex (const int index) const noexcept
{
	int n = 0;

	for (PopupMenu::MenuItemIterator iterator (currentMenu, true); iterator.next();)
	{
		auto& item = iterator.getItem();

		if (item.itemID != 0)
			if (n++ == index)
				return &item;
	}

	return nullptr;
}

int PopupSelector::getNumItems() const noexcept
{
	int n = 0;

	for (PopupMenu::MenuItemIterator iterator (currentMenu, true); iterator.next();)
	{
		auto& item = iterator.getItem();

		if (item.itemID != 0)
			n++;
	}

	return n;
}

String PopupSelector::getItemText (const int index) const
{
	if (auto* item = getItemForIndex (index))
		return item->text;

	return {};
}

int PopupSelector::getItemId (const int index) const noexcept
{
	if (auto* item = getItemForIndex (index))
		return item->itemID;

	return 0;
}

int PopupSelector::indexOfItemId (const int itemId) const noexcept
{
	if (itemId != 0)
	{
		int n = 0;

		for (PopupMenu::MenuItemIterator iterator (currentMenu, true); iterator.next();)
		{
			auto& item = iterator.getItem();

			if (item.itemID == itemId)
				return n;

			else if (item.itemID != 0)
				n++;
		}
	}

	return -1;
}

//==============================================================================
int PopupSelector::getSelectedItemIndex() const
{
	return indexOfItemId (currentId.getValue());
}

void PopupSelector::setSelectedItemIndex (const int index, const NotificationType notification)
{
	setSelectedId (getItemId (index), notification);
}

int PopupSelector::getSelectedId() const noexcept
{
	if (auto* item = getItemForId (currentId.getValue()))
		return item->itemID;

	return 0;
}

void PopupSelector::setSelectedId (const int newItemId, const NotificationType notification)
{
	auto* item = getItemForId (newItemId);
	auto newItemText = item != nullptr ? item->text : String();

	if (lastCurrentId != newItemId)
	{
		lastCurrentId = newItemId;
		currentId = newItemId;

		repaint();  // for the benefit of the 'none selected' text

		sendChange (notification);
	}
}

bool PopupSelector::selectIfEnabled (const int index)
{
	if (auto* item = getItemForIndex (index))
	{
		if (item->isEnabled)
		{
			setSelectedItemIndex (index);
			return true;
		}
	}

	return false;
}

bool PopupSelector::nudgeSelectedItem (int delta)
{
	for (int i = getSelectedItemIndex() + delta; isPositiveAndBelow (i, getNumItems()); i += delta)
		if (selectIfEnabled (i))
			return true;

	return false;
}

void PopupSelector::valueChanged (Value&)
{
	if (lastCurrentId != (int) currentId.getValue())
		setSelectedId (currentId.getValue());
}

//==============================================================================
void PopupSelector::paint (Graphics& g)
{
	getLookAndFeel().drawPopupSelectorButton (g, getWidth(), getHeight(), isButtonDown, menuActive, *this);
}

void PopupSelector::resized()
{
}

void PopupSelector::enablementChanged()
{
	if (! isEnabled())
		hidePopup();

	repaint();
}

void PopupSelector::colourChanged()
{
	lookAndFeelChanged();
}

void PopupSelector::parentHierarchyChanged()
{
	lookAndFeelChanged();
}

void PopupSelector::lookAndFeelChanged()
{
	repaint();
	resized();
}

//==============================================================================
bool PopupSelector::keyPressed (const KeyPress& key)
{
	if (key == KeyPress::upKey || key == KeyPress::leftKey)
	{
		nudgeSelectedItem (-1);
		return true;
	}

	if (key == KeyPress::downKey || key == KeyPress::rightKey)
	{
		nudgeSelectedItem (1);
		return true;
	}

	if (key == KeyPress::returnKey)
	{
		showPopupIfNotActive();
		return true;
	}

	return false;
}

bool PopupSelector::keyStateChanged (const bool isKeyDown)
{
	// only forward key events that aren't used by this component
	return isKeyDown
			&& (KeyPress::isKeyCurrentlyDown (KeyPress::upKey)
				|| KeyPress::isKeyCurrentlyDown (KeyPress::leftKey)
				|| KeyPress::isKeyCurrentlyDown (KeyPress::downKey)
				|| KeyPress::isKeyCurrentlyDown (KeyPress::rightKey));
}

//==============================================================================
void PopupSelector::focusGained (FocusChangeType)    { repaint(); }
void PopupSelector::focusLost (FocusChangeType)      { repaint(); }

//==============================================================================
void PopupSelector::showPopupIfNotActive()
{
	if (! menuActive)
	{
		menuActive = true;

		// as this method was triggered by a mouse event, the same mouse event may have
		// exited the modal state of other popups currently on the screen. By calling
		// showPopup asynchronously, we are giving the other popups a chance to properly
		// close themselves
		MessageManager::callAsync ([safePointer = SafePointer<PopupSelector> { this }]() mutable { if (safePointer != nullptr) safePointer->showPopup(); });
		repaint();
	}
}

void PopupSelector::hidePopup()
{
	if (menuActive)
	{
		menuActive = false;
		PopupMenu::dismissAllActiveMenus();
		repaint();
	}
}

static void popupSelectorPopupMenuFinishedCallback (int result, PopupSelector* combo)
{
	if (combo != nullptr)
	{
		combo->hidePopup();

		if (result != 0)
			combo->setSelectedId (result);
	}
}

void PopupSelector::showPopup()
{
	if (! menuActive)
		menuActive = true;

	auto menu = currentMenu;

	if (menu.getNumItems() > 0)
	{
		auto selectedId = getSelectedId();

		for (PopupMenu::MenuItemIterator iterator (menu, true); iterator.next();)
		{
			auto& item = iterator.getItem();

			if (item.itemID != 0)
				item.isTicked = (item.itemID == selectedId);
		}
	}
	else
	{
//		return;
		menu.addItem (1, "No Choices", false, false);
	}

	auto& lf = getLookAndFeel();

	menu.setLookAndFeel (&lf);
	
	auto opts = lf.getOptionsForPopupSelectorMenu (*this);
	
	int h = targetPopupBounds.getHeight() * menu.getNumItems();

	float scale = Component::getApproximateScaleFactorForComponent (this);
//	scale = 1;
	Rectangle<float> rect = (targetPopupBounds - getPosition()).toFloat() ;
	bool upwards = false;//rect.getY() < -0;
//	rect.translate(0, -h);
	rect.setHeight(h);
	rect *= scale;
	rect = rect + getScreenPosition().toFloat();
//	rect = localAreaToGlobal(rect);
	
	rect = rect.withSize(rect.getWidth() / scale, rect.getHeight() / scale);
	
	// auto targetArea = options.getTargetScreenArea() / scaleFactor;
//        auto parentArea = getParentArea (target.getCentre()) / scaleFactor;
	if (!upwards)
		rect.translate(0, -rect.getHeight());

	h = std::ceil(rect.getHeight() / menu.getNumItems());
	
	auto dir = upwards ? PopupMenu::Options::PopupDirection::upwards : PopupMenu::Options::PopupDirection::downwards;
	
	opts = opts.withTargetComponent (this)
					   .withPreferredPopupDirection (dir)
					   .withItemThatMustBeVisible (getSelectedId())
					   .withInitiallySelectedItem (getSelectedId())
					   .withTargetScreenArea(rect.toNearestInt())
					   .withMinimumWidth (rect.getWidth())
////					   .withMaximumNumColumns (1)
					   .withStandardItemHeight (h)
	;

	
	
	
	menu.showMenuAsync (opts,
						ModalCallbackFunction::forComponent (popupSelectorPopupMenuFinishedCallback, this));
}

//==============================================================================
void PopupSelector::mouseDown (const MouseEvent& e)
{
	beginDragAutoRepeat (300);

	isButtonDown = isEnabled() && ! e.mods.isPopupMenu();

	if (isButtonDown && (e.eventComponent == this ))
		showPopupIfNotActive();
}

void PopupSelector::mouseDrag (const MouseEvent& e)
{
	beginDragAutoRepeat (50);

	if (isButtonDown && e.mouseWasDraggedSinceMouseDown())
		showPopupIfNotActive();
}

void PopupSelector::mouseUp (const MouseEvent& e2)
{
	if (isButtonDown)
	{
		isButtonDown = false;
		repaint();

		auto e = e2.getEventRelativeTo (this);

		if (reallyContains (e.getPosition(), true)
			 && (e2.eventComponent == this))
		{
			showPopupIfNotActive();
		}
	}
}

void PopupSelector::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
	if (! menuActive && scrollWheelEnabled && e.eventComponent == this && wheel.deltaY != 0.0f)
	{
		mouseWheelAccumulator += wheel.deltaY * 5.0f;

		while (mouseWheelAccumulator > 1.0f)
		{
			mouseWheelAccumulator -= 1.0f;
			nudgeSelectedItem (-1);
		}

		while (mouseWheelAccumulator < -1.0f)
		{
			mouseWheelAccumulator += 1.0f;
			nudgeSelectedItem (1);
		}
	}
	else
	{
		Component::mouseWheelMove (e, wheel);
	}
}

void PopupSelector::setScrollWheelEnabled (bool enabled) noexcept
{
	scrollWheelEnabled = enabled;
}
	
void PopupSelector::setPopupBoundsInDoc(int x, int y, int w, int h, Rectangle<float>& pd)
{
	float rx = (x - pd.getX()) / pd.getWidth();
	float ry = (y - pd.getY()) / pd.getHeight();
	float rw = w / pd.getWidth();
	float rh = h / pd.getHeight();

	auto* p = getParentComponent();
	w = p ? p->getWidth() : getWidth();
	h = p ? p->getHeight() : getHeight();
	setPopupBounds(rx * w, ry * h, rw * w, rh * h);
}
void PopupSelector::setPopupBoundsInDoc(int x, int y, int w, int h)
{
	float rx = x / Component::getDocW();
	float ry = y / Component::getDocH();
	float rw = w / Component::getDocW();
	float rh = h / Component::getDocH();

	auto* p = getParentComponent();
	w = p ? p->getWidth() : getWidth();
	h = p ? p->getHeight() : getHeight();
	setPopupBounds(rx*w, ry*h, rw*w, rh*h);
}

	
void PopupSelector::setPopupBounds(int x, int y, int w, int h)
{
	setPopupBounds({x, y, w, h});
}
void PopupSelector::setPopupBounds(Rectangle<int> rect)
{
//	rect -= getPosition();
	targetPopupBounds = rect;
}

//==============================================================================
void PopupSelector::addListener    (PopupSelector::Listener* l)    { listeners.add (l); }
void PopupSelector::removeListener (PopupSelector::Listener* l)    { listeners.remove (l); }

void PopupSelector::handleAsyncUpdate()
{
	Component::BailOutChecker checker (this);
	listeners.callChecked (checker, [this] (Listener& l) { l.popupSelectorChanged (this); });

	if (checker.shouldBailOut())
		return;

	if (onChange != nullptr)
		onChange();

	if (auto* handler = getAccessibilityHandler())
		handler->notifyAccessibilityEvent (AccessibilityEvent::valueChanged);
}

void PopupSelector::sendChange (const NotificationType notification)
{
	if (notification != dontSendNotification)
		triggerAsyncUpdate();

	if (notification == sendNotificationSync)
		handleUpdateNowIfNeeded();
}

//==============================================================================
class PopupSelectorAccessibilityHandler  : public AccessibilityHandler
{
public:
	explicit PopupSelectorAccessibilityHandler (PopupSelector& comboBoxToWrap)
		: AccessibilityHandler (comboBoxToWrap,
								AccessibilityRole::comboBox,
								getAccessibilityActions (comboBoxToWrap),
								{ std::make_unique<PopupSelectorValueInterface> (comboBoxToWrap) }),
		  comboBox (comboBoxToWrap)
	{
	}

	AccessibleState getCurrentState() const override
	{
		auto state = AccessibilityHandler::getCurrentState().withExpandable();

		return comboBox.isPopupActive() ? state.withExpanded() : state.withCollapsed();
	}

	String getTitle() const override  { return comboBox.getSelectedText(); }
	String getHelp() const override   { return comboBox.getTooltip(); }

private:
	class PopupSelectorValueInterface  : public AccessibilityTextValueInterface
	{
	public:
		explicit PopupSelectorValueInterface (PopupSelector& comboBoxToWrap)
			: comboBox (comboBoxToWrap)
		{
		}

		bool isReadOnly() const override                 { return true; }
		String getCurrentValueAsString() const override  { return comboBox.getSelectedText(); }
		void setValueAsString (const String&) override   {}

	private:
		PopupSelector& comboBox;

		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupSelectorValueInterface)
	};

	static AccessibilityActions getAccessibilityActions (PopupSelector& comboBox)
	{
		return AccessibilityActions().addAction (AccessibilityActionType::press,    [&comboBox] { comboBox.showPopup(); })
									 .addAction (AccessibilityActionType::showMenu, [&comboBox] { comboBox.showPopup(); });
	}

	PopupSelector& comboBox;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupSelectorAccessibilityHandler)
};

std::unique_ptr<AccessibilityHandler> PopupSelector::createAccessibilityHandler()
{
	return std::make_unique<PopupSelectorAccessibilityHandler> (*this);
}



}//namespace juce
