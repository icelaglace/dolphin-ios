// Copyright 2022 DolphiniOS Project
// SPDX-License-Identifier: GPL-2.0-or-later

#import "ConfigSoundViewController.h"

#import "FoundationStringUtil.h"

#import "Core/Config/MainSettings.h"

@interface ConfigSoundViewController ()

@end

@implementation ConfigSoundViewController

- (void)viewWillAppear:(BOOL)animated {
  [super viewWillAppear:animated];
  
  self.backendLabel.text = CppToFoundationString(Config::Get(Config::MAIN_AUDIO_BACKEND));
  
  int volume = Config::Get(Config::MAIN_AUDIO_VOLUME);
  self.volumeSlider.value = volume;
  
  [self volumeChanged:self];
  
  bool stretchingEnabled = Config::Get(Config::MAIN_AUDIO_STRETCH);
  self.stretchingSwitch.on = stretchingEnabled;
  [self.stretchingSwitch addValueChangedTarget:self action:@selector(stretchingChanged)];
  
  self.bufferSizeSlider.value = Config::Get(Config::MAIN_AUDIO_STRETCH_LATENCY);
  
  [self bufferSizeChanged:self];
}

- (IBAction)volumeChanged:(id)sender {
  int volume = (int)self.volumeSlider.value;
  
  Config::SetBaseOrCurrent(Config::MAIN_AUDIO_VOLUME, volume);
  self.volumeLabel.text = [NSString stringWithFormat:@"%d%%", volume];
}

- (void)stretchingChanged {
  bool stretchingEnabled = self.stretchingSwitch.on;
  
  Config::SetBaseOrCurrent(Config::MAIN_AUDIO_STRETCH, stretchingEnabled);
  self.bufferSizeSlider.enabled = stretchingEnabled;
}

- (IBAction)bufferSizeChanged:(id)sender {
  int bufferSize = (int)self.bufferSizeSlider.value;
  
  Config::SetBaseOrCurrent(Config::MAIN_AUDIO_STRETCH_LATENCY, bufferSize);
  self.bufferSizeLabel.text = [NSString stringWithFormat:@"%d ms", bufferSize];
}

@end