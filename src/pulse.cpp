#include "pulse.hpp"

#include <thread>
#include <math.h>

syshud_pulseaudio::syshud_pulseaudio(Glib::Dispatcher* input_callback, Glib::Dispatcher* output_callback) :
	input_callback(input_callback), output_callback(output_callback) {

	mainloop = pa_mainloop_new();
	mainloop_api = pa_mainloop_get_api(mainloop);
	pa_signal_init(mainloop_api);
	context = pa_context_new(mainloop_api, "syshud");
	pa_context_connect(context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
	pa_context_set_state_callback(context, context_state_callback, this);

	// Is this thread safe?
	std::thread([&]() {
		pa_mainloop_run(mainloop, NULL);
	}).detach();
}

void syshud_pulseaudio::quit(int ret = 0) {
	mainloop_api->quit(mainloop_api, ret);
}

syshud_pulseaudio::~syshud_pulseaudio() {
	quit(0);

	// Cleanup
	if (context) {
		pa_context_unref(context);
		context = NULL;
	}

	if (signal) {
		pa_signal_free(signal);
		pa_signal_done();
		signal = NULL;
	}

	if (mainloop) {
		pa_mainloop_free(mainloop);
		mainloop = NULL;
		mainloop_api = NULL;
	}
}

void syshud_pulseaudio::exit_signal_callback(pa_mainloop_api *m, pa_signal_event *e, int sig, void *userdata) {
	syshud_pulseaudio* pa = (syshud_pulseaudio*)userdata;
	if (pa) pa->quit();
}

void syshud_pulseaudio::context_state_callback(pa_context *c, void *userdata) {
	syshud_pulseaudio* pa = (syshud_pulseaudio*)userdata;

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;

		case PA_CONTEXT_READY:
			pa_context_get_server_info(c, server_info_callback, userdata);
			pa_context_set_subscribe_callback(c, subscribe_callback, userdata);
			pa_context_subscribe(c, static_cast<pa_subscription_mask_t>(PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE), NULL, NULL);
			break;

		case PA_CONTEXT_TERMINATED:
			pa->quit(0);
			break;

		case PA_CONTEXT_FAILED:
		default:
			std::fprintf(stderr, "Pulseaudio failed to connect.\n");
				pa->quit(1);
			break;
	}
}


void syshud_pulseaudio::subscribe_callback(pa_context *c, pa_subscription_event_type_t type, uint32_t idx, void *userdata) {
	unsigned facility = type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;

	pa_operation *op = NULL;

	if (facility == PA_SUBSCRIPTION_EVENT_SINK)
		op = pa_context_get_sink_info_by_index(c, idx, sink_info_callback, userdata);
	else if (facility == PA_SUBSCRIPTION_EVENT_SOURCE)
		op = pa_context_get_source_info_by_index(c, idx, source_info_callback, userdata);

	if (op)
		pa_operation_unref(op);

	if (facility == PA_SUBSCRIPTION_EVENT_SERVER)
		pa_context_get_server_info(c, server_info_callback, userdata);
}

void syshud_pulseaudio::sink_info_callback(pa_context *c, const pa_sink_info *i, int eol, void *userdata) {
	syshud_pulseaudio* pa = (syshud_pulseaudio*)userdata;
	if (!i)
		return;

	// TODO: see if this is necessary
	// if (strcmp(i->name, pa->output_name))
	// 	return;

	// Set new values
	pa->volume = roundf(((float)pa_cvolume_avg(&(i->volume)) / (float)PA_VOLUME_NORM) * 100.0f);
	pa->muted = i->mute;

	// Trigger an update if needed
	if (pa->last_output_name != i->name || pa->volume != pa->previous_volume || pa->muted != pa->previous_muted) {
		pa->output_callback->emit();

		pa->last_output_name = i->name;
		pa->previous_volume = pa->volume;
		pa->previous_muted = pa->muted;
	}
}

void syshud_pulseaudio::source_info_callback(pa_context *c, const pa_source_info *i, int eol, void *userdata) {
	syshud_pulseaudio* pa = (syshud_pulseaudio*)userdata;

	if (!i)
		return;

	// Set new values
	pa->volume = roundf(((float)pa_cvolume_avg(&(i->volume)) / (float)PA_VOLUME_NORM) * 100.0f);
	pa->muted = i->mute;

	// Trigger an update if needed
	if (pa->last_input_name != i->name || pa->volume != pa->previous_volume || pa->muted != pa->previous_muted) {
		pa->input_callback->emit();

		pa->last_input_name = i->name;
		pa->previous_volume = pa->volume;
		pa->previous_muted = pa->muted;
	}
}

void syshud_pulseaudio::server_info_callback(pa_context *c, const pa_server_info *i, void *userdata) {
	pa_context_get_source_info_by_name(c, i->default_source_name, source_info_callback, userdata);
	pa_context_get_sink_info_by_name(c, i->default_sink_name, sink_info_callback, userdata);
}
