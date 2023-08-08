//
//  UnlockForm.cpp
//  
//
//  Created by Isaac Roberts on 12/16/20.
//

// Created by Isaac Roberts


#include "UnlockForm.h"

struct Spinner  : public Component,
				  private Timer
{
	Spinner()                       { startTimer (1000 / 50); }
	void timerCallback() override   { repaint(); }

	void paint (Graphics& g) override
	{
		getLookAndFeel().drawSpinningWaitAnimation (g, Colours::darkgrey, 0, 0, getWidth(), getHeight());
	}
};

struct UnlockForm::OverlayComp  : public Component,
										private Thread,
										private Timer,
										private Button::Listener
{
	OverlayComp (UnlockForm& f, bool hasCancelButton = false, bool hasLinkButton=false)
		: Thread (String()), form (f)
	{
		result.succeeded = LicenseUnlock::UnlockResult::NotRun;
		email = form.emailBox.getText();
		password = form.passwordBox.getText();
		addAndMakeVisible (spinner);

		if (hasCancelButton)
		{
			cancelButton.reset (new TextButton (TRANS (/*Cancel*/ UnlockUtil::deob4("Gergip"))));
			addAndMakeVisible (cancelButton.get());
			cancelButton->addListener (this);
		}

		startThread (4);
	}

	~OverlayComp() override
	{
		stopThread (10000);
	}

	void lookAndFeelChanged() override
	{
		auto* laf = &getLookAndFeel();
		if (cancelButton)
			cancelButton->setLookAndFeel(laf);
		spinner.setLookAndFeel(laf);
	}
	
	void paint (Graphics& g) override
	{
		g.fillAll (Colours::white.withAlpha (0.97f));
		
		g.setColour (Colours::black);
		g.setFont (15.0f);

		g.drawFittedText (TRANS(/*Contacting XYZ...*/ UnlockUtil::deob1("Dpoubdujoh!YZ[///")).replace ("XYZ", form.status.getWebsiteName()),
						  getLocalBounds().reduced (20, 0).removeFromTop (proportionOfHeight (0.6f)),
						  Justification::centred, 5);
	}

	void resized() override
	{
		const int spinnerSize = 40;
		spinner.setBounds ((getWidth() - spinnerSize) / 2, proportionOfHeight (0.6f), spinnerSize, spinnerSize);

		if (cancelButton != nullptr)
			cancelButton->setBounds (getLocalBounds().removeFromBottom (50).reduced (getWidth() / 4, 5));
	}

	void run() override
	{
		result = form.status.runUnlock (email, password);
		startTimer (100);
	}

	void timerCallback() override
	{
		spinner.setVisible (false);
		stopTimer();

		if (result.errorMessage.isNotEmpty())
		{
			AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
											  TRANS(/*Registration Failed*/ UnlockUtil::deob3("Uhjlvwudwlrq#Idlohg")),
											  result.errorMessage);
		}
		else if (result.informativeMessage.isNotEmpty())
		{
			AlertWindow::showMessageBoxAsync (AlertWindow::InfoIcon,
											  TRANS(/*Registration Complete!*/ UnlockUtil::deob3("Uhjlvwudwlrq#Frpsohwh$")),
											  result.informativeMessage);
		}
		else if (result.urlToLaunch.isNotEmpty())
		{
			URL url (result.urlToLaunch);
			url.launchInDefaultBrowser();
		}

		// (local copies because we're about to delete this)
		const bool worked = result.succeeded;
		UnlockForm& f = form;

		delete this;

		if (worked)
			f.dismiss();
	}

	void buttonClicked (Button* button) override
	{
		if (button == cancelButton.get())
		{
			form.status.userCancelled();

			spinner.setVisible (false);
			stopTimer();

			delete this;
		}
	}

	UnlockForm& form;
	Spinner spinner;
	LicenseUnlock::UnlockResult result;
	String email, password;

	std::unique_ptr<TextButton> cancelButton;

	JUCE_LEAK_DETECTOR (UnlockForm::OverlayComp)
};

static juce_wchar getDefaultPasswordChar() noexcept
{
   #if JUCE_LINUX
	return 0x2022;
   #else
	return 0x25cf;
   #endif
}

UnlockForm::UnlockForm (LicenseUnlock& s,
									bool hasCancelButton,
									bool overlayHasCancelButton,
									bool hasLinkButton)
	: unlock(s),
	message (String(), unlock.titleString()),
	  passwordBox (String(), getDefaultPasswordChar()),
	  registerButton (unlock.unlockButtonText()),
	  cancelButton (UnlockUtil::deobfuscate ("Dbodfm")),
	  linkButton(s.linkButtonName()),
	  status (unlock),
	  showOverlayCancelButton (overlayHasCancelButton)
{
	// Please supply a message to tell your users what to do!

	
	
	assert (!hasCancelButton || !hasLinkButton);
	
	setOpaque (false);

	addAndMakeVisible (message);

	if (unlock.hasEmailField())
	{
		emailBox.setText (status.getUserEmail());
		emailBox.setJustification(Justification::centred);
		
		addAndMakeVisible (emailBox);
	}
	
	message.setJustificationType (Justification::centred);

	passwordBox.setJustification(Justification::centred);
	
	addAndMakeVisible (passwordBox);
	addAndMakeVisible (registerButton);

	if (hasCancelButton)
		addAndMakeVisible (cancelButton);
	else if (hasLinkButton)
	{
		linkButton.setURL(s.linkButtonURL());
		addAndMakeVisible(linkButton);
	}
	
	emailBox.setEscapeAndReturnKeysConsumed (false);
	passwordBox.setEscapeAndReturnKeysConsumed (false);

	registerButton.addShortcut (KeyPress (KeyPress::returnKey));

	registerButton.addListener (this);
	cancelButton.addListener (this);

	setWantsKeyboardFocus(true);
	
	lookAndFeelChanged();
	setSize (500, 250);

//	grabKeyboardFocus();
}

void UnlockForm::onStart()
{
	yeeter->amIFake=true;
	
	bool unlocked = status.start();
	if (unlocked)
	{
		dismiss();
	}
}

bool UnlockForm::keyPressed (const KeyPress &key)
{
	//Eat all keyboard input
	return true;
}
UnlockForm::~UnlockForm()
{
	unlockingOverlay.deleteAndZero();
	yeeter->owner=NULL;
	delete yeeter;
}

void UnlockForm::paint (Graphics& g)
{
	uint8 grey = 163;
	g.fillAll (Colour(grey, grey, grey, .81f));
	
//	g.setColour(Colours::black);
//	g.fillRect(inlayBounds);
}

void UnlockForm::resized()
{
	/* If you're writing a plugin, then DO NOT USE A POP-UP A DIALOG WINDOW!
	   Plugins that create external windows are incredibly annoying for users, and
	   cause all sorts of headaches for hosts. Don't be the person who writes that
	   plugin that irritates everyone with a nagging dialog box every time they scan!
	*/
	
	jassert (JUCEApplicationBase::isStandaloneApp() || findParentComponentOfClass<DialogWindow>() == nullptr);

	if (unlockingOverlay != nullptr)
	{
		if (yeeter->amIFake)
		{
			yeeter->fakeData++;
			unlockingOverlay->setBounds (inlayBounds);
		}
		else
		{
			
		}
	}
}

void UnlockForm::setXMargin(float pct, int minW, Rectangle<int> parentBounds)
{
	
	const int buttonHeight = 40;
	
	const int boxHeight = 24;
	
	bool hasEmail = unlock.hasEmailField();
	
	float totalHeight = buttonHeight + 20* (hasEmail ? 3 : 2) + 40 + boxHeight * (hasEmail ? 2 : 1);
	
	int diff = (parentBounds.getHeight()-totalHeight)/2;
	
	pct = std::min(pct, .4f);
	int margin = parentBounds.getWidth() * pct;
	
	
	auto r = Rectangle<int>(margin, diff-20, parentBounds.getWidth()-margin*2, totalHeight+20);
	
	auto buttonArea = r.removeFromBottom (buttonHeight);
	registerButton.changeWidthToFitText (buttonHeight);
	cancelButton.changeWidthToFitText (buttonHeight);
	linkButton.changeWidthToFitText(buttonHeight);
	
	const int gap = 20;
	bool secondButton = cancelButton.isVisible() || linkButton.isVisible();
	int secondWidth = secondButton ? ( gap + (cancelButton.isVisible() ? cancelButton.getWidth() : linkButton.getWidth())) : 0;
	buttonArea = buttonArea.withSizeKeepingCentre (registerButton.getWidth() + secondWidth,
												   buttonHeight);
	registerButton.setBounds (buttonArea.removeFromRight (registerButton.getWidth()));
	buttonArea.removeFromRight (gap);
	cancelButton.setBounds (buttonArea);
	linkButton.setBounds(buttonArea);

	r.removeFromBottom (20);

	// (force use of a default system font to make sure it has the password blob character)
	Font font (Font::getDefaultTypefaceForFont (Font (Font::getDefaultSansSerifFontName(),
													  Font::getDefaultStyle(),
													  5.0f)));

	passwordBox.setBounds (r.removeFromBottom (boxHeight));
//	passwordBox.setInputRestrictions (64);
	passwordBox.setFont (font);
	r.removeFromBottom (20);

	if (hasEmail)
	{
		emailBox.setBounds (r.removeFromBottom (boxHeight));
		emailBox.setInputRestrictions (512);
		emailBox.setFont (font);
		r.removeFromBottom (20);
	}

	message.setBounds (r.removeFromBottom(40));
	
	if (unlockingOverlay != nullptr)
	{
		unlockingOverlay->setBounds (inlayBounds);
	}
}

void UnlockForm::lookAndFeelChanged()
{
	Colour labelCol (findColour (TextEditor::backgroundColourId).contrasting (0.5f));

	emailBox.setTextToShowWhenEmpty (unlock.emailFormName(), labelCol);
	passwordBox.setTextToShowWhenEmpty (unlock.passwordFormName(), labelCol);
}

void UnlockForm::showBubbleMessage (const String& text, Component& target)
{
	bubble.reset (new BubbleMessageComponent (500));
	bubble->setColour(BubbleMessageComponent::backgroundColourId, Colours::white);
	addChildComponent (bubble.get());

	AttributedString attString;
	attString.append (text, Font (16.0f));

	bubble->showAt (getLocalArea (&target, target.getLocalBounds()),
					attString, 500,  // numMillisecondsBeforeRemoving
					true,  // removeWhenMouseClicked
					false); // deleteSelfAfterUse
}

void UnlockForm::buttonClicked (Button* b)
{
	if (b == &registerButton)
		attemptRegistration();
	else if (b == &cancelButton)
		dismiss();
	yeeter->fakeData--;
}

void UnlockForm::makeOverlayComp()
{
	unlockingOverlay = new OverlayComp (*this, showOverlayCancelButton);
	unlockingOverlay->setLookAndFeel(&getLookAndFeel());
	addAndMakeVisible (unlockingOverlay);
}

void UnlockForm::attemptRegistration()
{
	if (unlockingOverlay == nullptr)
	{
//		showBubbleMessage (TRANS ("Nah !"), passwordBox);
//		return;

		if (unlock.hasEmailField() && emailBox.getText().trim().length() < 3)
		{
			String validWhat = unlock.emailFormName().toLowerCase();
			showBubbleMessage (TRANS (/*Please enter a valid */ UnlockUtil::deob3("Sohdvh#hqwhu#d#ydolg#") + validWhat+"!"), emailBox);
			return;
		}

		if (passwordBox.getText().trim().length() < 3)
		{
			String validWhat = unlock.passwordFormName().toLowerCase();
			showBubbleMessage (TRANS (/*Please enter a valid */ UnlockUtil::deob5("Uqjfxj%jsyjw%f%{fqni%") + validWhat+"!"), passwordBox);
			return;
		}

		if (unlock.hasEmailField())
			status.setUserEmail (emailBox.getText());
		else
			status.setUserEmail("");
		
		makeOverlayComp();
		resized();
		unlockingOverlay->enterModalState();
	}
}

void UnlockForm::forceUnlock()
{
//	setVisible(false);
}

void UnlockForm::dismiss()
{
	setVisible(false);
//    delete this;
}

void UnlockForm::setVisible(bool vis)
{
	//bypasses setVis is already unlocked
	if (vis)
	{
		if (status.isUnlocked())
		{
			return;
		}
	}
	Component::setVisible(vis);
}
   

void UnlockForm::visibilityChanged()
{
	if (isVisible())
	{
		if (!yeeter)
			yeeter = new Yeeter(this);
	}
}
UnlockForm::Yeeter::Yeeter(UnlockForm* f)
: fakeData(0),
  amIFake(true),
  owner(f)
{
	//Freezes if they remove "addAndMakeVisible(unlockForm);" from the main component
	while (!owner->isVisible())
		owner->repaint();
}
