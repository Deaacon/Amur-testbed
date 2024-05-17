function csrf(headers) {
    let matches = document.cookie.match(new RegExp(
        "(?:^|; )csrftoken=([^;]*)"
    ));
    if (!('headers' in headers)) {
        headers['headers'] = {};
    }
    headers['headers']['X-CSRFToken'] = matches ? decodeURIComponent(matches[1]) : undefined;
    return headers;
}


export class TestbedAPI {
    constructor(base_url) {
        this._base_url = base_url;
    }

    async pinsInfo() {
        let url = this._base_url + '';
        return await fetch(url, csrf({
            method: 'GET'
        })).then(response => response.json());
    }

    async upload(source) {
        let url = this._base_url + 'upload/';
        return await fetch(url, csrf({
            method: 'POST',
            body: source
        })).then(response => response.json());
    }

    async monitorStart() {
        let url = this._base_url + 'monitor/';
        return await fetch(url, csrf({
            method: 'POST',
            body: 'start'
        })).then(response => response.json());
    }

    async monitorStop() {
        let url = this._base_url + 'monitor/';
        return await fetch(url, csrf({
            method: 'POST',
            body: 'stop'
        })).then(response => response.json());
    }

    async getCurrentState() {
        let url = this._base_url + 'io/';
        return await fetch(url, csrf({
            method: 'GET'
        })).then(response => response.json());
    }

    async getStateHistory() {
        let url = this._base_url + 'io/';
        return await fetch(url, csrf({
            method: 'GET'
        })).then(response => response.json());
    }

    async setInputs(inputs) {
        let url = this._base_url + 'io/';
        return await fetch(url, csrf({
            method: 'POST',
            body: JSON.stringify(inputs)
        })).then(response => response.json());
    }
}

export let testbedApi = new TestbedAPI('/session/api/');
window.testbedApi = testbedApi;
