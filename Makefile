NAME = bit-crusher
LV2_BUNDLE = $(NAME).lv2

all: lv2

lv2: clean-lv2
	mkdir $(LV2_BUNDLE)
	gcc -shared -fPIC -DPIC src/$(NAME).c src/lv2.c -o $(LV2_BUNDLE)/$(NAME).so
	cp manifest.ttl $(NAME)-mono.ttl $(NAME)-stereo.ttl $(LV2_BUNDLE)

clean: clean-lv2

clean-lv2:
	rm -rf $(LV2_BUNDLE)
