program gfxcore2wrapper_test;

uses
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  gfxcore in 'gfxcore.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
