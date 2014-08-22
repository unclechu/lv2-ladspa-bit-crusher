NAME = bit-crusher
LV2_BUNDLE = $(NAME).lv2
LADSPA_BUNDLE = ladspa

all: lv2 ladspa

lv2: clean-lv2
	mkdir $(LV2_BUNDLE)
	gcc -shared -fPIC -DPIC src/$(NAME).c src/lv2.c -o $(LV2_BUNDLE)/$(NAME).so
	cp manifest.ttl $(NAME)-mono.ttl $(NAME)-stereo.ttl $(LV2_BUNDLE)

ladspa: clean-ladspa
	mkdir $(LADSPA_BUNDLE)
	gcc -shared -fPIC -DPIC src/$(NAME).c src/ladspa.c -o $(LADSPA_BUNDLE)/$(NAME).so

clean: clean-lv2 clean-ladspa

clean-lv2:
	rm -rf $(LV2_BUNDLE)

clean-ladspa:
	rm -rf $(LADSPA_BUNDLE)
