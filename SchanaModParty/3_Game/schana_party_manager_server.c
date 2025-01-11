class SchanaPartyManagerServer  extends Managed {
	protected autoptr map<string, autoptr set<string>> configurations;
	protected bool canSendInfo = true;

	protected bool canGenerateParties = true;
	protected bool canGeneratePositions = true;
	protected bool canGenerateHealth = true;
	
	
	protected bool canGeneratePartiesThread = true; //To prevent possbilty of threads conflicting??

	autoptr map<string, autoptr set<string>> parties;
	autoptr map<string, vector> player_positions;
	autoptr map<string, float> player_healths;

	void SchanaPartyManagerServer () {
		SchanaPartyUtils.LogMessage ("Server Init " + SCHANA_PARTY_VERSION);
		configurations = new map<string,autoptr set<string>> ();
		GetRPCManager ().AddRPC ("SchanaModParty", "ServerRegisterPartyRPC", this, SingleplayerExecutionType.Both);

		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.SendInfo, 10000, true);
		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.ResetSendInfoLock, GetSchanaPartyServerSettings ().GetSendInfoFrequency () * 1000, true);

		int logFrequency = GetSchanaPartyServerSettings ().GetLogFrequency ();
		if (logFrequency > 0) {
			GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.LogParties, logFrequency * 1000, true);
		}
	}

	void ~SchanaPartyManagerServer () {
		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).Remove (this.SendInfo);
		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).Remove (this.ResetSendInfoLock);

		int logFrequency = GetSchanaPartyServerSettings ().GetLogFrequency ();
		if (logFrequency > 0) {
			GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).Remove (this.LogParties);
		}
	}

	protected void ResetSendInfoLock () {
		canSendInfo = true;
	}

	protected void LogParties () {
		string result;
		JsonSerializer ().WriteToString (parties, false, result);
		SchanaPartyUtils.Warn ("Parties " + result);
	}

	void ServerRegisterPartyRPC (CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target) {
			SchanaPartyUtils.Trace ("ServerRegisterPartyRPC Start");
		Param2<string, array<string>> data;
		if (!ctx.Read (data))
			return;
		array<string> ids = new array<string>;
		ids.Copy (data.param2);
		if (SchanaPartyUtils.WillLog (SchanaPartyUtils.INFO)) {
			string result;
			JsonSerializer ().WriteToString (data, false, result);
			SchanaPartyUtils.Info ("ServerRegisterPartyRPC " + result);
		}

		ServerRegisterParty (data.param1, ids);
	}

	protected void ServerRegisterParty (string key, array<string> ids) {
		SchanaPartyUtils.Trace ("ServerRegisterParty Start");
		SchanaPartyUtils.Info ("Register " + ids.Count ().ToString () + " to " + key);
		auto party_members = new set<string> ();
		foreach (string id : ids) {
			party_members.Insert (id);
		}

		string result;

		if (SchanaPartyUtils.WillLog (SchanaPartyUtils.TRACE)) {
			JsonSerializer ().WriteToString (configurations, false, result);
			SchanaPartyUtils.Trace ("ServerRegisterParty Before " + result);
		}

		configurations.Set (key, party_members);

		if (SchanaPartyUtils.WillLog (SchanaPartyUtils.TRACE)) {
			JsonSerializer ().WriteToString (configurations, false, result);
			SchanaPartyUtils.Trace ("ServerRegisterParty After " + result);
		}

		SendInfo ();
	}

	autoptr map<string, autoptr set<string>> GetParties () {
		SchanaPartyUtils.Trace ("GetParties Start");
		if (!canGenerateParties) {
			SchanaPartyUtils.Trace ("GetParties Returned Cached Parties");
			return parties;
		}
		canGenerateParties = false;
		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.ThreadGenerateParties, 50);
		if (parties){
			SchanaPartyUtils.Trace ("GetParties Returned Cached Parties and requested New Cache");
			return parties;
		}
		SchanaPartyUtils.Trace ("GetParties First Run Cache");
		GenerateParties ();
		return parties;
	}

	void ThreadGenerateParties (){
			SchanaPartyUtils.Trace ("ThreadGenerateParties Start");
			thread GenerateParties ();
	}

	void GenerateParties (){
		if (canGeneratePartiesThread){ //Prevents Two Threads from running at the same time?
			canGeneratePartiesThread = false;
			SchanaPartyUtils.Trace ("GenerateParties Start");
			int maxPartyRefreshRate = GetSchanaPartyServerSettings ().GetMaxPartyRefreshRate ();
			GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.ResetPartiesRefreshRate, maxPartyRefreshRate * 1000, false);
			parties = new map<string, autoptr set<string>> ();

			for (int i = 0; i < configurations.Count (); ++i) {
				auto validated_party_ids = new set<string>;
				SchanaPartyUtils.Trace ("validated party id " + configurations.GetKey (i));
				if (configurations.GetElement (i) && configurations.GetElement (i).Count () > 0){
					foreach (string member_id : configurations.GetElement (i)) {
						if (configurations.Contains (member_id) && configurations.Get (member_id).Find (configurations.GetKey (i)) != -1) {
							SchanaPartyUtils.Trace ("validated party id " + configurations.GetKey (i) + " party_ids " +  member_id);
							validated_party_ids.Insert (member_id);
						} else if (configurations.Contains (member_id) && GetSchanaPartyServerSettings ().GetAdminIds ().Find (configurations.GetKey (i)) != -1) {
							SchanaPartyUtils.Trace ("validated party id " + configurations.GetKey (i) + " party_ids " +  member_id);
							validated_party_ids.Insert (member_id);
						}
					}
				}
				SchanaPartyUtils.Trace ("parties Insert id " + configurations.GetKey (i));
				if (configurations.GetKey (i) && validated_party_ids){
					parties.Set (configurations.GetKey (i), validated_party_ids);
				}
			}
			SchanaPartyUtils.Trace ("GenerateParties End");
			canGeneratePartiesThread = true;
		} else {
			SchanaPartyUtils.Trace ("GenerateParties Can't Run another thread already running");
		}
	}

	void ResetPartiesRefreshRate () {
		SchanaPartyUtils.Trace ("ResetPartiesRefreshRate Start");
		canGenerateParties = true;
	}

	autoptr array<DayZPlayer> GetPartyPlayers (string id) {
		SchanaPartyUtils.Trace ("GetPartyPlayers Start");
		map<string, DayZPlayer> id_map = new map<string, DayZPlayer> ();
		array<Man> game_players = new array<Man>;
		GetGame ().GetPlayers (game_players);
		int i;
		for (i = 0; i < game_players.Count (); ++i) {
			DayZPlayer player = DayZPlayer.Cast (game_players.Get (i));
			if (player && player.GetIdentity () && player.IsAlive ()) {
				id_map.Insert (player.GetIdentity ().GetId (), player);
			}
		}
		
		SchanaPartyUtils.Trace ("id_map Count: " + id_map.Count ());
		array<DayZPlayer> players = new array<DayZPlayer>;
		SchanaPartyUtils.Trace ("member_ids Start");
		set<string> member_ids = GetParties ().Get (id);
		if (member_ids) {
			SchanaPartyUtils.Trace ("member_ids Count: " + member_ids.Count ());
			for (i = 0; i < member_ids.Count (); ++i) {
				string member_id = member_ids.Get (i);
				SchanaPartyUtils.Trace ("member_ids member_id: " + member_id);
				if (id_map.Contains (member_id)) {
				SchanaPartyUtils.Trace ("id_map Contains: " + member_id);
					DayZPlayer plr = DayZPlayer.Cast (id_map.Get (member_id));
					if (plr) {
						SchanaPartyUtils.Trace ("players Insert: " + member_id);
						players.Insert (plr);
					}
				}
			}
		}
		return players;
	}

	protected map<string, vector> GetPositions () {
			SchanaPartyUtils.Trace ("GetPositions Start");
		if (!canGeneratePositions) {
			return player_positions;
		}

		SchanaPartyUtils.Trace ("GetPositions Start ");
		player_positions = new map<string, vector> ();

		int maxPartyRefreshRate = GetSchanaPartyServerSettings ().GetMaxPartyRefreshRate ();
		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.ResetPositionsRefreshRate, maxPartyRefreshRate * 1000, false);
		canGeneratePositions = false;

		array<Man> players = new array<Man>;
		GetGame ().GetPlayers (players);

            for (int i = 0; i < players.Count (); ++i ) {
                DayZPlayer player = DayZPlayer.Cast (players.Get (i));
                if (player && player.GetIdentity () && player.IsAlive ()) {
                    player_positions.Insert (player.GetIdentity ().GetId (), player.GetPosition ());
                }
            }
		SchanaPartyUtils.Trace ("GetPositions Finish");

		return player_positions;
	}

	void ResetPositionsRefreshRate () {
			SchanaPartyUtils.Trace ("ResetPositionsRefreshRate Start");
		canGeneratePositions = true;
	}

	protected map<string, float> GetHealths () {
			SchanaPartyUtils.Trace ("GetHealths Start");
		if (!canGenerateHealth) {
			return player_healths;
		}
		SchanaPartyUtils.Trace ("GetHealths Start 2");
		player_healths = new map<string, float> ();

		int maxPartyRefreshRate = GetSchanaPartyServerSettings ().GetMaxPartyRefreshRate ();
		GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.ResetHealthsRefreshRate, maxPartyRefreshRate * 1000, false);
		canGenerateHealth = false;

		array<Man> players = new array<Man>;
		GetGame ().GetPlayers (players);

            for (int i = 0; i < players.Count (); ++i ) {
                DayZPlayer player = DayZPlayer.Cast (players.Get (i));
                if (player && player.GetIdentity () && player.IsAlive ()) {
                    player_healths.Insert (player.GetIdentity ().GetId (), player.GetHealth ("", ""));
                }
            }
		SchanaPartyUtils.Trace ("GetHealths Finish ");

		return player_healths;
	}

	void ResetHealthsRefreshRate () {
			SchanaPartyUtils.Trace ("ResetHealthsRefreshRate Start");
		canGenerateHealth = true;
	}

	protected void SendInfo () {
		SchanaPartyUtils.Trace ("SendInfo Start");
		if (canSendInfo) {
			SchanaPartyUtils.Trace ("Call SendInfoThread");
			thread SendInfoThread ();
			
			int sendInfoFrequency = GetSchanaPartyServerSettings ().GetSendInfoFrequency ();
			GetGame ().GetCallQueue (CALL_CATEGORY_SYSTEM).CallLater (this.ResetSendInfoLock, sendInfoFrequency * 1000, false);
			
			canSendInfo = false;
			SchanaPartyUtils.Trace ("SendInfo End");
		} else {
			
			SchanaPartyUtils.Trace ("SendInfo End (Not Run)");
		}
	}

    protected void SendInfoThread () {
		SchanaPartyUtils.Trace ("SendInfoThread Start");

		SendPartyInfo ();
		SendPlayersInfo ();
		
		SchanaPartyUtils.Trace ("SendInfoThread End");
	}


	protected void SendPartyInfo () {

			SchanaPartyUtils.Trace ("SendPartyInfo Start");
		auto id_map = new map<string, DayZPlayer> ();

		array<Man> players = new array<Man>;
		GetGame ().GetPlayers (players);

            for (int i = 0; i < players.Count (); ++i ) {
                DayZPlayer player = DayZPlayer.Cast (players.Get (i));
                if (player && player.GetIdentity () && player.IsAlive ()) {
                    id_map.Insert (player.GetIdentity ().GetId (), player);
                }
            }
			
		auto positions = GetPositions ();
		auto server_healths = GetHealths ();
		auto s_parties = GetParties ();
		if (!s_parties){
			return;
		}
		if (SchanaPartyUtils.WillLog (SchanaPartyUtils.DEBUG)) {
			string result;
			JsonSerializer ().WriteToString (positions, false, result);
			SchanaPartyUtils.Debug ("Positions " + result);
			JsonSerializer ().WriteToString (server_healths, false, result);
			SchanaPartyUtils.Debug ("Healths " + result);
			JsonSerializer ().WriteToString (s_parties, false, result);
			SchanaPartyUtils.Debug ("Parties " + result);
		}

		int maxPartySize = GetSchanaPartyServerSettings ().GetMaxPartySize ();
		foreach (auto id, auto party_ids : s_parties) {
			SchanaPartyUtils.Trace ("SendInfo Begin " + id);
			if (!positions.Contains (id)) {
				configurations.Remove (id);
			} else {
				DayZPlayer ply = DayZPlayer.Cast (id_map.Get (id));
				if (ply && ply.GetIdentity () && ply.IsAlive ()) {
					SendPartyInfoToPlayer (id, party_ids, maxPartySize, positions, server_healths, ply.GetIdentity ());
				}
			}
			SchanaPartyUtils.Trace ("SendInfo End " + id);
		}
	}

	protected void SendPartyInfoToPlayer (string id, set<string> party_ids, int maxPartySize, map<string, vector> positions, map<string, float> server_healths, PlayerIdentity player) {
			SchanaPartyUtils.Trace ("SendPartyInfoToPlayer Start");
		auto ids = new array<string>;
		auto locations = new array<vector>;
		auto healths = new array<float>;
		foreach (string party_id : party_ids) {
			if (positions.Contains (party_id) && (maxPartySize < 0 || ids.Count () < maxPartySize)) {
				ids.Insert (party_id);
				locations.Insert (positions.Get (party_id));
				healths.Insert (server_healths.Get (party_id));
			}
		}
		auto info = new Param3<array<string>, array<vector>, array<float>> (ids, locations, healths);

		if (SchanaPartyUtils.WillLog (SchanaPartyUtils.INFO)) {
			string result;
			JsonSerializer ().WriteToString (info, false, result);
			SchanaPartyUtils.Info ("SendInfo to " + id + " " + result);
		}

		PlayerIdentity plyr = PlayerIdentity.Cast (player);
		if (plyr) {
			GetRPCManager ().SendRPC ("SchanaModParty", "ClientUpdatePartyInfoRPC", info, false, plyr);
		} else {
			SchanaPartyUtils.Warn ("SendInfo failed to " + id);
		}
	}

	protected void SendPlayersInfo () {
			SchanaPartyUtils.Trace ("SendPlayersInfo Start");
		
		auto id_map = new map<string, DayZPlayer> ();
		auto all_player_ids = new array<string>;
		auto all_player_names = new array<string>;

		array<Man> players = new array<Man>;
		GetGame ().GetPlayers (players);

            for (int i = 0; i < players.Count (); ++i ) {
                DayZPlayer player = DayZPlayer.Cast (players.Get (i));
                if (player && player.GetIdentity () && player.IsAlive () && GetSchanaPartyServerSettings ().GetAdminIds ().Find (player.GetIdentity ().GetId ()) == -1) {
					all_player_ids.Insert (player.GetIdentity ().GetId ());
					all_player_names.Insert (player.GetIdentity ().GetName ());
                }
            }
		
		auto all_player_info = new Param2<array<string>, array<string>> (all_player_ids, all_player_names);

		if (SchanaPartyUtils.WillLog (SchanaPartyUtils.DEBUG)) {
			string result;
			JsonSerializer ().WriteToString (all_player_info, false, result);
			SchanaPartyUtils.Debug ("SendPlayers " + result);
		}

		GetRPCManager ().SendRPC ("SchanaModParty", "ClientUpdatePlayersInfoRPC", all_player_info);
	}
}

static ref SchanaPartyManagerServer g_SchanaPartyManagerServer;
static SchanaPartyManagerServer GetSchanaPartyManagerServer () {
	if (g_Game.IsServer () && !g_SchanaPartyManagerServer) {
		g_SchanaPartyManagerServer = new SchanaPartyManagerServer;
	}
	return g_SchanaPartyManagerServer;
}
