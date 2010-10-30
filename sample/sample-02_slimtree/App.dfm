object Form1: TForm1
  Left = 309
  Top = 215
  Width = 578
  Height = 451
  Caption = 'Slim-Tree Demo'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object RGQueryType: TRadioGroup
    Left = 272
    Top = 8
    Width = 137
    Height = 120
    Caption = ' Query Type '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ItemIndex = 0
    Items.Strings = (
      'Nearest Query'
      'Range Query'
      'Point Query')
    ParentFont = False
    TabOrder = 0
  end
  object GBQueryCity: TGroupBox
    Left = 8
    Top = 8
    Width = 249
    Height = 120
    Caption = ' Query City '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object Label1: TLabel
      Left = 8
      Top = 25
      Width = 46
      Height = 16
      Caption = 'Name:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label2: TLabel
      Left = 8
      Top = 72
      Width = 60
      Height = 16
      Caption = 'Latitude:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label3: TLabel
      Left = 128
      Top = 72
      Width = 73
      Height = 16
      Caption = 'Longitude:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ECityName: TEdit
      Left = 8
      Top = 43
      Width = 233
      Height = 24
      TabOrder = 0
      Text = 'São Carlos-SP'
    end
    object ECityLatitude: TEdit
      Left = 8
      Top = 90
      Width = 100
      Height = 24
      TabOrder = 1
      Text = '-22,02'
    end
    object ECityLongitude: TEdit
      Left = 128
      Top = 90
      Width = 100
      Height = 24
      TabOrder = 2
      Text = '47,89'
    end
  end
  object GBResult: TGroupBox
    Left = 8
    Top = 176
    Width = 553
    Height = 225
    Caption = ' Result '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object MResult: TMemo
      Left = 8
      Top = 18
      Width = 537
      Height = 199
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 0
    end
  end
  object GBQueryParameter: TGroupBox
    Left = 424
    Top = 8
    Width = 137
    Height = 120
    Caption = ' Query Parameter '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    object Label4: TLabel
      Left = 16
      Top = 70
      Width = 54
      Height = 16
      Caption = 'Radius:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label5: TLabel
      Left = 16
      Top = 22
      Width = 13
      Height = 16
      Caption = 'k:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object CSNearestNumber: TCSpinEdit
      Left = 16
      Top = 40
      Width = 73
      Height = 26
      TabStop = True
      MaxValue = 1
      MinValue = 1
      ParentColor = False
      TabOrder = 0
      Value = 1
    end
    object ERadius: TEdit
      Left = 16
      Top = 88
      Width = 73
      Height = 24
      TabOrder = 1
      Text = '0'
    end
  end
  object BRun: TBitBtn
    Left = 357
    Top = 140
    Width = 80
    Height = 28
    Caption = '&Run'
    Default = True
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ModalResult = 6
    ParentFont = False
    TabOrder = 4
    OnClick = BRunClick
    NumGlyphs = 2
  end
  object BSample: TBitBtn
    Left = 133
    Top = 140
    Width = 80
    Height = 28
    Caption = '&Sample'
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ModalResult = 4
    ParentFont = False
    TabOrder = 5
    OnClick = BSampleClick
    NumGlyphs = 2
  end
  object BClear: TBitBtn
    Left = 245
    Top = 140
    Width = 80
    Height = 28
    Caption = '&Clear'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ModalResult = 3
    ParentFont = False
    TabOrder = 6
    OnClick = BClearClick
    NumGlyphs = 2
  end
  object MainMenu1: TMainMenu
    Left = 384
    Top = 65528
    object Exit1: TMenuItem
      Caption = 'Exit'
      OnClick = Exit1Click
    end
    object BuildSlimTree1: TMenuItem
      Caption = 'Build'
      object SlimTree1: TMenuItem
        Caption = 'Slim-Tree'
        OnClick = SlimTree1Click
      end
    end
    object About1: TMenuItem
      Caption = 'About'
      OnClick = About1Click
    end
  end
end
