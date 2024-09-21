export class TestbedAPI {
    constructor(base_url) {
        this._base_url = base_url;
    }

    _csrf(options) {
        const matches = document.cookie.match(new RegExp(
            "(?:^|; )csrftoken=([^;]*)"
        ));
        if (!options.hasOwnProperty('headers')) {
            options['headers'] = {};
        }
        options['headers']['X-CSRFToken'] = matches ? decodeURIComponent(matches[1]) : undefined;
        return options;
    }

    async _doRequest(endpoint, options) {
        let url = this._base_url;
        if (endpoint) {
            url += endpoint + '/';
        }
        return await fetch(url, this._csrf(options))
            .then((response) => {
                if (response.ok) {
                    return response.json();
                } else {
                    let status_json = {error: response.statusText, status: response.status};
                    return response.json()
                        .then((response_json) => Object.assign(status_json, response_json))
                        .catch((reason) => status_json);
                }
            })
            .catch((reason) => {
                console.error(reason);
                return {reason: reason ? reason : "UNKNOWN ERROR"};
            });
    }

    async getCameraFeed() {
        const url = this._base_url + 'camera_feed/';
        return await fetch(url, csrf({
            method: 'GET',
            headers: {
                'Content-Type': "multipart/x-mixed-replace; boundary=frame",
            },
        })).then((response) => response);
    }

    async pinsInfo() {
        return await this._doRequest('', {method: 'GET'});
    }

    async upload(source) {
        return await this._doRequest('upload', {method: 'POST', body: source});
    }

    async getMonitorState() {
        return await this._doRequest('monitor', {method: 'GET'})
            .then((json) => json.error ? json : json.running);
    }

    async setMonitorState(running) {
        return await this._doRequest('monitor',
            {method: 'POST', body: running ? 'start' : 'stop'});
    }

    async monitorStart() {
        return await this.setMonitorState(true);
    }

    async monitorStop() {
        return await this.setMonitorState(false);
    }

    async getCurrentState() {
        return await this._doRequest('io', {method: 'GET'});
    }

    async getStateHistory() {
        return await this._doRequest('io', {method: 'GET'});
    }

    async setInputs(inputs) {
        return await this._doRequest('io', {method: 'POST', body: JSON.stringify(inputs)});
    }
}

export const testbedApi = new TestbedAPI('/session/api/');
window.testbedApi = testbedApi;
