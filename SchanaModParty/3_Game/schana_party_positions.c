class SchanaPartyPositions  extends Managed {
    protected autoptr map<string, vector> positions;

    void SchanaPartyPositions () {
        positions = new map<string, vector> ();
    }

    map<string, vector> Get () {
        return positions;
    }

    void Replace (array<string> ids, array<vector> server_positions) {
        positions.Clear ();
        for (int i = 0; i < ids.Count (); ++i) {
            positions.Insert (ids.Get (i), server_positions.Get (i));
        }
    }
}