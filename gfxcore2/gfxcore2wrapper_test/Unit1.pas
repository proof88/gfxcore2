unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, gfxcore, StdCtrls, Math, Types;

const
  TEST_EPSILON = 0.001;

type
  TForm1 = class(TForm)
    Memo1: TMemo;
    Button1: TButton;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
var
  rgba: TRGBA;
  objint: integer;
  objname: tstr40;
begin
  if ( tmcsInitGraphix(0, false, 0, 32, 24, false, 0) = 0) then
    memo1.Lines.Add('tmcsInitGraphix() PASS')
  else
    memo1.Lines.Add('tmcsInitGraphix() FAIL');

  // boolean
  if ( tmcsInitialized() ) then
    memo1.Lines.Add('tmcsInitialized() PASS')
  else
    memo1.Lines.Add('tmcsInitialized() FAIL');

  // single
  tmcsSetCameraPos(1.0, 2.0, 3.0);

  if ( EqualsValue = CompareValue(tmcsGetCameraX(), 1.0, TEST_EPSILON) ) then
    memo1.Lines.Add('tmcsGetCameraX() PASS!')
  else
    memo1.Lines.Add('tmcsGetCameraX() FAIL!');

  if ( EqualsValue = CompareValue(tmcsGetCameraY(), 2.0, TEST_EPSILON) ) then
    memo1.Lines.Add('tmcsGetCameraY() PASS!')
  else
    memo1.Lines.Add('tmcsGetCameraY() FAIL!');

  if ( EqualsValue = CompareValue(tmcsGetCameraZ(), 3.0, TEST_EPSILON) ) then
    memo1.Lines.Add('tmcsGetCameraZ() PASS!')
  else
    memo1.Lines.Add('tmcsGetCameraZ() FAIL!');

  // double
  tmcsSetCameraFov(56.7);
  if ( EqualsValue = CompareValue(tmcsGetCameraFov(), 56.7, TEST_EPSILON) ) then
    memo1.Lines.Add('tmcsGetCameraFov() PASS!')
  else
    memo1.Lines.Add('tmcsGetCameraFov() FAIL!');

  // byte
  tmcsSetMotionBlurUpdateRate(34);

  // TRGBA
  tmcsSetMotionBlurColor(200, 150, 100, 50);
  rgba := tmcsGetMotionBlurColor();

  if ( rgba.red = 200 ) then
    memo1.Lines.Add('rgba.red PASS!')
  else
    memo1.Lines.Add('rgba.red FAIL!');

  if ( rgba.green = 150 ) then
    memo1.Lines.Add('rgba.green PASS!')
  else
    memo1.Lines.Add('rgba.green FAIL!');

  if ( rgba.blue = 100 ) then
    memo1.Lines.Add('rgba.blue PASS!')
  else
    memo1.Lines.Add('rgba.blue FAIL!');

  if ( rgba.alpha = 50 ) then
    memo1.Lines.Add('rgba.alpha PASS!')
  else
    memo1.Lines.Add('rgba.alpha FAIL!');

  // word, tstr40
  objint := tmcsCreatePlane(2, 2);
  tmcsSetName(Word (objint), 'alma');

  objname := tmcsGetName(Word (objint));
  if ( objname = 'alma' ) then
    memo1.Lines.Add('tmcsGetName() PASS!')
  else
    memo1.Lines.Add('tmcsGetName() FAIL!');

  // tglconst
  tmcsSetObjectBlendMode(Word(objint), GL_ONE, GL_ONE);

  if ( tmcsShutdownGraphix() = 0 ) then
    memo1.Lines.Add('tmcsShutdownGraphix() PASS!')
  else
    memo1.Lines.Add('tmcsShutdownGraphix() FAIL!');
end;

end.
