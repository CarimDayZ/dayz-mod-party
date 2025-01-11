modded class MissionGameplay extends MissionBase {
    protected ref SchanaPartyMenu m_SchanaPartyMenu;
	
	protected int SchanaPingTimerMax = 0;
	
	override void OnMissionStart(){
		super.OnMissionStart();
        GetRPCManager ().AddRPC ("SchanaModParty", "SchanaPartyModSettingsRPC", this, SingleplayerExecutionType.Both);
		SchanaPartyUtils.LogMessage ("Requesting settings from server");
		GetRPCManager ().SendRPC ("SchanaModParty", "SchanaPartyModSettingsRPC", new Param1< SchanaModPartyServerSettings >( NULL ), true, NULL);
	}
	
	
	void SchanaPartyModSettingsRPC( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target ) {
		Param1< SchanaModPartyServerSettings > data  //Player ID, Icon
		if ( !ctx.Read( data ) ) return;
		g_SchanaPartyServerSettings = data.param1;
	}
	
    override void OnInit () {
        super.OnInit ();
        GetSchanaPartyManagerClient ();
        GetSchanaPartyMarkerManagerClient ();
    }

    override void OnMissionFinish () {
        super.OnMissionFinish ();
    }

    override void OnUpdate (float timeslice) {
        super.OnUpdate (timeslice);
		int NowTime = GetGame().GetTime();
        Man player = GetGame ().GetPlayer ();

        if (player && !player.IsUnconscious () && !GetSchanaPartyMarkerManagerClient ().IsInitialized ()) {
            GetSchanaPartyMarkerManagerClient ().Init ();
        }

        if (GetUApi () && !m_UIManager.IsMenuOpen (MENU_CHAT_INPUT)) {
            Input input = GetGame ().GetInput ();
            if (input.LocalPress ("UAUIBack", false)) {
                if (m_SchanaPartyMenu != NULL && GetGame ().GetUIManager ().GetMenu () == m_SchanaPartyMenu) {
                    m_SchanaPartyMenu.SchanaPartyMenuSetOpen (false);
                    GetGame ().GetUIManager ().HideScriptedMenu (m_SchanaPartyMenu);
                    SchanaPartyUnlockControls ();
                }
            }

            if (input.LocalPress ("UASchanaPartyMenu", false)) {
                if (m_SchanaPartyMenu) {
                    if (m_SchanaPartyMenu.SchanaPartyMenuIsOpen ()) {
                        //Hide Menu
                        m_SchanaPartyMenu.SchanaPartyMenuSetOpen (false);
                        GetGame ().GetUIManager ().HideScriptedMenu (m_SchanaPartyMenu);
                        SchanaPartyUnlockControls ();
                    } else if (GetGame ().GetUIManager ().GetMenu () == NULL) {
                        //Show Menu
                        GetGame ().GetUIManager ().ShowScriptedMenu (m_SchanaPartyMenu, NULL);
                        m_SchanaPartyMenu.SchanaPartyMenuSetOpen (true);
                        SchanaPartyLockControls ();
                    }
                } else if (GetGame ().GetUIManager ().GetMenu () == NULL && m_SchanaPartyMenu == null) {
                    //Create Menu
                    SchanaPartyLockControls ();
                    m_SchanaPartyMenu = SchanaPartyMenu.Cast (GetUIManager ().EnterScriptedMenu (SCHANA_PARTY_MENU, null));
                    m_SchanaPartyMenu.SchanaPartyMenuSetOpen (true);
                }
            }

            if (input.LocalPress ("UASchanaPartyPing", false) && NowTime > SchanaPingTimerMax && GetGame ().GetUIManager ().GetMenu () == NULL) {
				SchanaPingTimerMax = NowTime + 700;
                vector position = SchanaPartyGetRaycastPosition ();
                if (position != vector.Zero) {
                    auto marker_client = GetSchanaPartyMarkerManagerClient ();
                    auto new_marker = new SchanaPartyMarkerInfo (marker_client.GetNextName (), position);
                    marker_client.Add (new_marker);
                }
            }

            if (input.LocalPress ("UASchanaPartyPingClear", false) && NowTime > SchanaPingTimerMax && GetGame ().GetUIManager ().GetMenu () == NULL) {
				SchanaPingTimerMax = NowTime + 1200;
                GetSchanaPartyMarkerManagerClient ().Reset ();
            }
        }
    }

    protected vector SchanaPartyGetRaycastPosition () {
        vector begin = GetGame ().GetCurrentCameraPosition () + GetGame ().GetCurrentCameraDirection ();
        vector end = begin + GetGame ().GetCurrentCameraDirection () * 8000;
        vector contactPos;
        vector contactDir;
        int contactComponent;

        if (DayZPhysics.RaycastRV (begin, end, contactPos, contactDir, contactComponent)) {
            return contactPos;
        }

        return vector.Zero;
    }

    protected void SchanaPartyLockControls () {
        GetGame ().GetMission ().PlayerControlDisable (INPUT_EXCLUDE_MOUSE_ALL);
        GetGame ().GetUIManager ().ShowUICursor (true);
    }

    protected void SchanaPartyUnlockControls () {
        GetGame ().GetMission ().PlayerControlEnable (false);
        GetGame ().GetInput ().ResetGameFocus ();
        GetGame ().GetUIManager ().ShowUICursor (false);
    }
}