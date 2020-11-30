# vote_manager
Dtabase manager for secure voting

Manager of e-Voting database  is a part of eVoting server.

It manages the personal data of voters and generates voting invites as pictures (jpeg-files) with QR-codes.
Since the server locks database for security purposes during its operation, it must be turned off when the manager is working.

After first start you must specify path to:
- Registrator's database (vote_s/reg_data/reg.db3): press button <...> on top right of App
- Voter's list of candidates (vote_s/vot_data/vot.txt): press <File> button on 'Candidates' page
- Tor's onion adress (vote_s/tor/hidden_service/hostname): press <Srv=> button on left buttom of 'Edit' page.

Before start press 'Open DB' button. Path fiel will be green on success.

Manage list of candidates:
- go to 'Candidates' tab;
- press 'Load' for display list of candidates;
- add/remove candidates (can be up to 12 candidates in list);
- press 'Save' 

Manage list of voters:
- go to 'Edit' tab;
- press 'New' for add voter or set voter's number and press 'Load' for edit.
- Enter name and ID of voter;
- Press "Auto' button on the rigth of password field for generate unique password (or enter passord manually);
- Press 'Save' button
- Press 'QR-code' button for entering QR-code tab with QR-code picture
- On this tab press 'Save' for saving peronal invite as jpeg and data files in 'qr_codes' folder.
- repeat for next voter;

Find voters in database:
- go to 'Find' tab;
- select search mode: 'Match', 'Start from' or 'Contain';
- Select field for search: 'Name' or 'ID';
- Enter string for search; 
- Press 'Serach'
- Double click on finded voter for open  on 'Edit' tab;

This is a project for Borland C++ Builder 6.
Source code is on C (core: QR-code generator, jpeg-compressor, sqlite etc.) and C++ (GUI).

Developer: Viktoria Malevanchenko, student of NMTU named acad.Yu.Bugai, Ukraine, Poltava, 2020
MaitTo: vika_nmtu@protonmail.com
