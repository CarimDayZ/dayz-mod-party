modded class MissionServer extends MissionBase {
    override void OnInit () {
        super.OnInit ();
		SchanaPartyUtils.LogMessage("OnInit");
		GetSchanaPartyServerSettings ();
        GetSchanaPartyManagerServer ();
        GetSchanaPartyMarkerManagerServer ();
		GetRPCManager ().AddRPC ( "SchanaModParty", "SchanaPartyModSettingsRPC", this, SingeplayerExecutionType.Both );
	}
	
	void SchanaPartyModSettingsRPC( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target ) {
		PlayerIdentity RequestedBy = PlayerIdentity.Cast(sender);
		if (RequestedBy){
			SchanaPartyUtils.Trace ("Settings Requested By " + RequestedBy.GetId());
			GetRPCManager ().SendRPC ("SchanaModParty", "SchanaPartyModSettingsRPC", new Param1< SchanaModPartyServerSettings >( GetSchanaPartyServerSettings () ), true, RequestedBy);
		}
	}
	
}