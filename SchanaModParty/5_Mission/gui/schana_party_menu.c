class SchanaPartyMenu extends UIScriptedMenu {
	protected bool m_SchanaPartyIsInitialized;
	protected bool m_SchanaPartyMenuIsOpen;

	protected TextListboxWidget m_SchanaPartyPlayerList;
	protected TextListboxWidget m_SchanaPartyPartyList;
	protected ButtonWidget m_SchanaPartyButtonAdd;
	protected ButtonWidget m_SchanaPartyButtonRemove;

	protected ref map<string, string> member_sorting_map;

	void SchanaPartyMenu () {
		member_sorting_map = new map<string, string> ();
	}

	override Widget Init () {
		if (!m_SchanaPartyIsInitialized) {
			layoutRoot = GetGame ().GetWorkspace ().CreateWidgets ("SchanaModParty/GUI/Layouts/manager.layout");
			m_SchanaPartyPlayerList = TextListboxWidget.Cast (layoutRoot.FindAnyWidget ("Players"));
			m_SchanaPartyPartyList = TextListboxWidget.Cast (layoutRoot.FindAnyWidget ("Party"));
			m_SchanaPartyButtonAdd = ButtonWidget.Cast (layoutRoot.FindAnyWidget ("ButtonAdd"));
			m_SchanaPartyButtonRemove = ButtonWidget.Cast (layoutRoot.FindAnyWidget ("ButtonRemove"));

			m_SchanaPartyIsInitialized = true;
		}

		SchanaPartyUpdateLists ();

		return layoutRoot;
	}

	/*
	static void PopulateTextList(TextListboxWidget w, int count, string col1, string col2 = "", string col3 = "", string col4 = "", string col5 = "")
	{
    	TextListboxWidget twidget = TextListboxWidget.Cast(w);
    	for (int i = 0; i < count; i++)
    	{
	        string text = col1 + i;
	        int row = twidget.AddItem(text, NULL, 0);
	        if (col2 != "")
	        {
	            text = col2 + i;
	            twidget.AddItem(text, NULL, 1, row);
        	}
        	if (col3 != "")
        	{
	            text = col3 + i;
	            twidget.AddItem(text, NULL, 2, row);
        	}
        	if (col4 != "")
        	{
	            text = col4 + i;
	            twidget.AddItem(text, NULL, 3, row);
        	}
        	if (col5 != "")
        	{
	            text = col5 + i;
	            twidget.AddItem(text, NULL, 4, row);
        	};
    	};
	};
	*/

	void ~SchanaPartyMenu () {
		GetGame ().GetCallQueue (CALL_CATEGORY_GUI).Remove (this.SchanaPartyUpdateLists);

		GetGame ().GetUIManager ().Back ();
		g_Game.GetUIManager ().ShowCursor (true);
		g_Game.GetUIManager ().ShowUICursor (false);
		GetGame ().GetInput ().ResetGameFocus ();

		if (layoutRoot) {
			layoutRoot.Unlink ();
		}
	}

	override void OnShow () {
		super.OnShow ();
		GetGame ().GetCallQueue (CALL_CATEGORY_GUI).CallLater (this.SchanaPartyUpdateLists, 500, true);

    	GetGame().GetMission().GetHud().Show(false);
    	PPEffects.SetBlurInventory(0.5);
	}

	override void OnHide () {
		super.OnHide ();
		GetGame ().GetCallQueue (CALL_CATEGORY_GUI).Remove (this.SchanaPartyUpdateLists);

		g_Game.GetUIManager ().ShowCursor (true);
		g_Game.GetUIManager ().ShowUICursor (false);
		GetGame ().GetInput ().ResetGameFocus ();

	    GetGame().GetMission().GetHud().Show(true);
	    GetGame().GetUIManager().Back();
	    PPEffects.SetBlurInventory(0);
	    Close();
	}

	override bool OnClick (Widget w, int x, int y, int button) {
		int selectedRow;
		Param1<string> id;

		switch (w) {
			case m_SchanaPartyButtonAdd:
				selectedRow = m_SchanaPartyPlayerList.GetSelectedRow ();
				if (selectedRow == -1) {
					break;
				}
				m_SchanaPartyPlayerList.GetItemData (selectedRow, 0, id);
				SchanaPartyUtils.LogMessage ("Menu add " + id.param1);
				GetSchanaPartyManagerClient ().AddPlayerToParty (id.param1);
				SchanaPartyUpdateLists ();
				return true;
				break;

			case m_SchanaPartyButtonRemove:
				selectedRow = m_SchanaPartyPartyList.GetSelectedRow ();
				if (selectedRow == -1) {
					break;
				}
				m_SchanaPartyPartyList.GetItemData (selectedRow, 0, id);
				SchanaPartyUtils.LogMessage ("Menu remove " + id.param1);
				GetSchanaPartyManagerClient ().RemovePlayerFromParty (id.param1);
				m_SchanaPartyPartyList.SelectRow (selectedRow - 1);
				SchanaPartyUpdateLists ();
				return true;
				break;
		}
		SchanaPartyUpdateLists ();
		return super.OnClick (w, x, y, button);
	}

	void SchanaPartyUpdateLists () {
		if (m_SchanaPartyIsInitialized) {
			SchanaPartyUpdatePartyList ();
			SchanaPartyUpdatePlayerList ();
			SchanaPartyUpdatePartyStatus ();
		}
	}

	void SchanaPartyUpdatePartyList () {
		member_sorting_map.Clear ();
		string member_name;
		int insert_row = 0;

		auto members = GetSchanaPartySettings ().GetMembers ();
		foreach (auto id : members) {
			member_name = GetSchanaPartySettings ().GetName (id);
			member_name.ToLower ();
			member_sorting_map.Insert (member_name + id, id);
		}

		auto sorted_keys = member_sorting_map.GetKeyArray ();
		sorted_keys.Sort ();
		foreach (auto sorted_key : sorted_keys) {
			string member_id = member_sorting_map.Get (sorted_key);
			member_name = GetSchanaPartySettings ().GetName (member_id);
			if (insert_row < m_SchanaPartyPartyList.GetNumItems ()) {
				m_SchanaPartyPartyList.SetItem (insert_row, member_name, new Param1<string> (member_id), 0);
			} else {
				m_SchanaPartyPartyList.AddItem (member_name, new Param1<string> (member_id), 0);
			}
			insert_row++;
		}

		while (insert_row < m_SchanaPartyPartyList.GetNumItems ()) {
			m_SchanaPartyPartyList.SelectRow (-1);
			m_SchanaPartyPartyList.RemoveRow (insert_row);
		}

		if (m_SchanaPartyPartyList.GetSelectedRow () >= insert_row) {
			m_SchanaPartyPartyList.SelectRow (insert_row - 1);
		}

		//PopulateTextList(m_SchanaPartyPartyList, 40, "Party_");
	}

	void SchanaPartyUpdatePlayerList () {
		member_sorting_map.Clear ();
		int insert_row = 0;

		auto onlinePlayers = GetSchanaPartyManagerClient ().GetOnlinePlayers ();
		foreach (auto id, auto player_name : onlinePlayers) {
			player_name.ToLower ();
			member_sorting_map.Insert (player_name + id, id);
		}

		auto sorted_keys = member_sorting_map.GetKeyArray ();
		sorted_keys.Sort ();
		foreach (auto sorted_key : sorted_keys) {
			string player_id = member_sorting_map.Get (sorted_key);

			if (insert_row < m_SchanaPartyPlayerList.GetNumItems ()) {
				m_SchanaPartyPlayerList.SetItem (insert_row, onlinePlayers.Get (player_id), new Param1<string> (player_id), 0);
			} else {
				m_SchanaPartyPlayerList.AddItem (onlinePlayers.Get (player_id), new Param1<string> (player_id), 0);
			}
			insert_row++;
		}

		while (insert_row < m_SchanaPartyPlayerList.GetNumItems ()) {
			m_SchanaPartyPlayerList.RemoveRow (insert_row);
		}

		if (m_SchanaPartyPlayerList.GetSelectedRow () >= insert_row) {
			m_SchanaPartyPlayerList.SelectRow (insert_row - 1);
		}

		//PopulateTextList(m_SchanaPartyPlayerList, 40, "Player_");
	}

	void SchanaPartyUpdatePartyStatus () {
		int selectedRow;
		int i;
		Param1<string> id;
		for (i = 0; i < m_SchanaPartyPartyList.GetNumItems (); ++i) {
			m_SchanaPartyPartyList.GetItemData (i, 0, id);

			if (GetSchanaPartyManagerClient ().IsPartyMemberOnline (id.param1)) {
				// Green A400
				m_SchanaPartyPartyList.SetItemColor (i, 0, 0xFF00E676);
			} else if (GetSchanaPartyManagerClient ().IsPartyMemberOnlineButNotMutual (id.param1)) {
				// Light Blue A400
				m_SchanaPartyPartyList.SetItemColor (i, 0, 0xFF00B0FF);
			} else {
				// Gray 400
				m_SchanaPartyPartyList.SetItemColor (i, 0, 0xFFBDBDBD);
			}
		}
		for (i = 0; i < m_SchanaPartyPlayerList.GetNumItems (); ++i){
			// Gray 400
			m_SchanaPartyPlayerList.SetItemColor (i, 0, 0xFFBDBDBD);
		}
		selectedRow = m_SchanaPartyPartyList.GetSelectedRow ();
		if (selectedRow != -1){
			m_SchanaPartyPartyList.SetItemColor (selectedRow, 0, 0xDDEDC131);
		}
		selectedRow = m_SchanaPartyPlayerList.GetSelectedRow ();
		if (selectedRow != -1){
				m_SchanaPartyPlayerList.SetItemColor (selectedRow, 0, 0xDDEDC131);
		}
	}

	bool SchanaPartyMenuIsOpen () {
		return m_SchanaPartyMenuIsOpen;
	}

	void SchanaPartyMenuSetOpen (bool open) {
		m_SchanaPartyMenuIsOpen = open;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
	    ColorHighlight (w);
	    return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
	    ColorNormal (w);
	    return true;
	}

	protected void ColorHighlight(Widget w)
	{
	    if (!w)
	        return;

	    ButtonSetColor(w, ARGB(0, 0, 0, 0));
	    ButtonSetTextColor(w, ARGB(255, 235, 168, 68));
	    ImagenSetColor(w, ARGB(255, 235, 168, 68));
	};

	protected void ColorNormal(Widget w)
	{
	    if (!w)
	        return;

	    ButtonSetColor(w, ARGB(0, 0, 0, 0));
	    ButtonSetTextColor(w, ARGB(255, 255, 255, 255));
	    ImagenSetColor(w, ARGB(255, 255, 255, 255));
	};

	protected void ButtonSetColor(Widget w, int color)
	{
	    if (!w)
	        return;
	        
	    Widget panel = w.FindWidget(w.GetName() + "_panel");

	    if (panel)
	    {
	        panel.SetColor(color);
	    };
	};

	protected void ButtonSetTextColor(Widget w, int color)
	{
	    if (!w)
	        return;

	    TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
	                
	    if (label)
	    {
	        label.SetColor(color);
	    };
	};

	void ImagenSetColor( Widget w, int color )
	{
		if( !w )
			return;
		
		Widget panel = w.FindWidget( w.GetName() + "_img" );
		
		if( panel )
		{
			panel.SetColor( color );
		}
	}
}