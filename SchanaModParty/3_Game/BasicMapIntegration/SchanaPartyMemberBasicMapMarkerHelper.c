class SchanaPartyMemberBasicMapMarkerHelper extends Managed {
#ifdef BASICMAP
    ref BasicMapMarker marker;
#endif

    void SetPosition (vector position) {
#ifdef BASICMAP
        marker.SetPosition (position);
#endif
    }

    void SetName (string name) {
#ifdef BASICMAP
        marker.Name = name;
#endif
    }

    void SchanaPartyMemberBasicMapMarkerHelper () {
#ifdef BASICMAP
        marker = new BasicMapMarker ("", "0 0 0", "BasicMap\\gui\\images\\player.paa", { 128, 80, 244 });
        BasicMap ().AddMarker (SchanaPartyBasicMapAPI.GROUP_ID_MEMBERS, marker);
#endif
    }

    void ~SchanaPartyMemberBasicMapMarkerHelper () {
#ifdef BASICMAP
        BasicMap ().RemoveMarker (SchanaPartyBasicMapAPI.GROUP_ID_MEMBERS, marker);
		delete marker;
#endif
    }

}