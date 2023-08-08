//
//  SMParamGroup.h
//  
//
//  Created by Isaac Roberts on 8/11/22.
//

// Created by Isaac Roberts 


class SMParamGroup : public PresetObject
{
private:
	class SoloParam : public ParameterBase {
	public:
		SoloParam(SMParamGroup*);
		virtual ~SoloParam() {}
		
		float getValue() const override {
			return (value+1)/float(ct+1);
		}
		void setValue (float newValue) override {
			int v = std::round(newValue * (ct+1))-1;
			if (v< 0) v = -1;
			if (v>=ct) v = ct-1;
			set(v);
		}
		float getDefaultValue() const override {
			return 0;
		}
		String getName (int maximumStringLength) const override {
			return "S";
		}
		String getFullName() const override {
			return "Solo";
		}
		
		operator int() { return value; }
		//Units. "Hz" or "%"
		String getLabel() const override { return ""; }
		float getValueForText (const String& text) const override { return 0; }
		
		String getIdForSave() const override { return "S"; }
		void save(XmlElement* element) const override;
		void load(XmlElement* element) override;
		void loadAsDefault(XmlElement* element) override;

		void updateBackend() override {}
		void updateFrontend(void* data) override { update_(); }

		void clearBackendCallbacks() override {}
		void clearFrontendCallbacks() override {group->clearSoloCallbacks(); }
		
		void resetToDefault() override {}
		void randomize() override {}
		
		void set(int set) {
			if (value != set)
			{
				if (set < -1 ) {
					jassertfalse;
					set = -1;
				}
				if (set >= ct) {
					jassertfalse;
					set = ct-1;
				}
				int prev = value;
				value = set;
				group->setSoloFlat(value, prev);
			}
		}
		void setFromGroup(int set) {
			value = set;
		}
	protected:
		void update_() { group->setSoloFlat(value);}
		
	protected:
		SMParamGroup* group;
		int ct;
		int value;
//		std::function<void(bool)> uiCallback;
	};
	class MuteParam : public ParameterBase {
	public:
		MuteParam();
		virtual ~MuteParam() {}
		void setBasics(SMParamGroup*, int n);
		
		float getValue() const override {
			return float(muted);
		}
		void setValue (float newValue) override {
			muted = newValue > .5;
		}
		float getDefaultValue() const override {
			return 0;
		}
		
		operator bool(){ return muted; }
		void setFromGroup(bool s) {
			muted = s;
			uiCallback(muted);
		}
		
		String getName (int maximumStringLength) const override { return "M"+String(n); }
		String getFullName() const override { return "Mute "+String(n);}

		//Units. "Hz" or "%"
		String getLabel() const override { return ""; }
		float getValueForText (const String& text) const override { return 0; }
		
		String getIdForSave() const override { return "M"+String(n); }
		void save(XmlElement* element) const override;
		void load(XmlElement* element) override;
		void loadAsDefault(XmlElement* element) override;

		void updateBackend() override {}
		void updateFrontend(void* data) override { uiCallback(muted); }

		void clearBackendCallbacks() override {}
		void clearFrontendCallbacks() override { uiCallback = [&](bool){}; }
		
		void resetToDefault() override {}
		void randomize() override {}
	protected:
		void update_() { group->setMute(n, muted); }
		
	protected:
		SMParamGroup* group;
		int n;
		bool muted;
	public:
		std::function<void(bool)> uiCallback;
	};
	
public:
	SMParamGroup(int ct);
	virtual ~SMParamGroup();
	
//	void addToProcessor(AudioProcessor* p) override;
	
	bool isMuted(int n);
	bool isSoloed() { return solo != -1; }
	
	int getSolo() { return int(solo);}
	bool getPureMute(int n) { return bool(mute[n]); }
	bool isPureMuted(int n) { return bool(mute[n]); }
	
	void connectSoloButton(int n, Button* button, int radioId, bool flip=false);
	void connectMuteButton(int n, Button* button, bool flip=false) ;

	
	void connectSoloButton(int n, Button& button, int radioId, bool flip=false) {
		connectSoloButton(n, &button, radioId, flip);
	}
	void connectMuteButton(int n, Button& button, bool flip=false) {
		connectMuteButton(n, &button, flip);
	}
	
	int getCt() { return ct; }
	void setName(String set) { xmlName = set; }
protected:
	
	String getXmlName() override { return xmlName; }
	
	int getParamAmt() override {
		return ct + 1;
	}
	ParameterBase* getParam(int n) override {
		if (n==0)
			return &solo;
		n--;
		if (n < ct)
			return &mute[n];
	
		jassertfalse;
		return NULL;
	}
	
	
	int getSubPresetAmt() override { return 0;}
	PresetObject* getSubPreset(int n) override { jassertfalse; return NULL; }
	
	void clearSoloCallbacks();

	void setSoloFlat(int n);
	void setSoloFlat(int n, int prev);
	void setSolo(int n, bool set);
	void setMute(int n, bool set);

//	void updateSolo(int n, int sourceN, bool sourceSolo);
//	void updateMute(int n, int sourceN, bool sourceSolo);
	
protected:
	int ct;
	
	SoloParam solo;
	MuteParam* mute;
	
	String xmlName;
	
	std::function<void(bool)>* soloCallbacks;

private:
	SMParamGroup(const SMParamGroup& other)=delete;
	SMParamGroup& operator=(const SMParamGroup& other)=delete;
	
	friend class SoloParam;
	friend class MuteParam;
};
