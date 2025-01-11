class SchanaPartyMarkerInfo  extends Managed {
    protected string name;
    protected vector position;

    void SchanaPartyMarkerInfo (string markerName, vector markerPosition) {
        name = markerName;
        position = markerPosition;
    }

    string GetName () {
        return name;
    }

    vector GetPosition () {
        return position;
    }
}